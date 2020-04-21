#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * Функции init_window_and_renderer  и destroy_window_and_renderer 
 * позволяют опосредованно, через указатели перезаписать переменные из main
 * и укорачивают код основной программы.
 * */
char const *init_window_and_renderer(SDL_Window **window_indirect, SDL_Renderer **renderer_indirect, int width, int height, char const *ascii_title) {
	char const *error_message = "";

	SDL_ClearError();
	*window_indirect = SDL_CreateWindow(ascii_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
	if (*(error_message = SDL_GetError())) goto END;
	SDL_ClearError();
	*renderer_indirect = SDL_CreateRenderer(*window_indirect, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*(error_message = SDL_GetError())) goto END;

END:
	return error_message;
}

void destroy_window_and_renderer(SDL_Window *window, SDL_Renderer *renderer) {
	if (NULL != renderer) SDL_DestroyRenderer(renderer);
	if (NULL != window) SDL_DestroyWindow(window);
}
/*******************************************************************************/

//Структура IKI_Circle моделирует круг радиуса .r с центром в точке (.cx , .cy)
typedef struct _IKI_Circle {
	int cx, cy, r;
} IKI_Circle;

//Рисуем круг, заполняя его линиямих
void IKI_DrawCircle(SDL_Renderer *renderer, IKI_Circle const *circle) {
	for (int h = 0; h < circle->r; ++h) {
		int x_offset = (int)sqrt(circle->r * circle->r - h * h);
		SDL_RenderDrawLine(renderer, circle->cx - x_offset, circle->cy - h, circle->cx + x_offset, circle->cy - h);
		SDL_RenderDrawLine(renderer, circle->cx - x_offset, circle->cy + h, circle->cx + x_offset, circle->cy + h);
	}
}



//Структура IKI_Scene соединяет все объекты, которые нужно нарисовать в один 'ящик' -- это удобнее, меньше передавать параметров в функцию рисования, особенно если элементов нарисовать нужно много...
typedef struct _IKI_Scene {
	SDL_Rect const *stady_rect;
	IKI_Circle *moving_circle;
} IKI_Scene;


//Эта функция выполняет рисование круга и прямоугольника
void draw_scene(SDL_Renderer *renderer, IKI_Scene const *scene) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x99, 0xFF);
	SDL_RenderFillRect(renderer, scene->stady_rect);

	SDL_SetRenderDrawColor(renderer, 0xFF, 0x33, 0x33, 0xFF);
	IKI_DrawCircle(renderer, scene->moving_circle); //Вызываем функцию, которую написали сами - это не функция SDL!!!

	SDL_RenderPresent(renderer);
}

Uint32 timer_function(Uint32 interval, void *_params) {
	SDL_UserEvent userevent;         //Создаём новый экземпляр структуры SDL_UserEvent, чтобы затем отправить в очередь
	userevent.type = SDL_USEREVENT;  //Каждое сообщение должно иметь указание на свой тип, чтобы его можно было диагностировать в if или switch
	userevent.code = 0;
    
    //Очередь может содержать любое сообщение, потому мы предварительно 'погружаем' сообщение в union
    SDL_Event event;            //SDL_Event - это union (объединение), в котором по выбору может лежать одного из множетсва типов сообщений
	event.type = SDL_USEREVENT; //.type в объединении служит для диагностики типа сообщения, которое в него положили, значение должно строго соответствовать типу сообщения!!!
	event.user = userevent;     //Сообщение типа SDL_UserEvent мы можем положить в поле .user; как и любой union SDL_Event имеет разные поля доступа к данным разных типов

	SDL_PushEvent(&event); //Экземпляр объединения SDL_Event уже можно положить в очередь сообщений
	return interval;
}

int main(int agc, char **argv) {
    //Инициализируем SDL
	if (0 != SDL_Init(SDL_INIT_VIDEO)) {
		printf("Can't initialize SDL.\nCause: %s\n", SDL_GetError());
		goto END;
	}

	//Основная программа
	{
		SDL_Window *main_window = NULL;
		SDL_Renderer *main_renderer = NULL;
		SDL_TimerID timer_id = 0;  //Каждый таймер имеет свой отдельный номер (идентификатор), он необходим для последующей остановки таймера

		int const SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600, CLOUD_SIZE = 300;
		const char *error_message = "";

		if (*(error_message = init_window_and_renderer(&main_window, &main_renderer, SCREEN_WIDTH, SCREEN_HEIGHT, "SDL Render Draw Example"))) {
			printf("Can't create widnow or renderer.\nCause: %s\n", error_message);
			goto CLEANUP;
		}

		//Создаём экземпляры структур, которые соответствуют объектам на экране
		SDL_Rect const stady_rect = { 100, 50, 600, 200 };   //покоящийся прямоугольник
		IKI_Circle moving_circle = { 150, 450, 50 };         //движущийся круг
		IKI_Scene scene = { &stady_rect, &moving_circle };   //полная сцена = круг + прямоугольник
		
		timer_id = SDL_AddTimer(33, timer_function, NULL); //Создаём таймер: запускается отдельный 'поток', в котором через каждые 33 миллисекунды вызывается функция timer_function

		//Основной цикл обработки сообщений
		SDL_Event e;
		bool proceed = true;
		while (proceed) {
			if (SDL_PollEvent(&e)) {
				switch (e.type) {
					case SDL_QUIT:
					{
						proceed = false;
						break;
					}

					case SDL_USEREVENT:
					{
						draw_scene(main_renderer, &scene); //Для сокращения кода в основном цикле рисование вынесено в отдельную функцию; см. выше
						break;
					}
				}
			}
			moving_circle.cx += 1; //Эффект движения достигается за счёт изменения положения объекта, в данном случае положения центра круга
			SDL_Delay(1); //Создаём задержку, чтобы движение не было слишком быстрым
  		}


	CLEANUP:
		destroy_window_and_renderer(main_window, main_renderer);
		SDL_RemoveTimer(timer_id); //Освобождаем таймер, возвращаем ресурс потока системе
	}

END:
	SDL_Quit();
	return 0;
}