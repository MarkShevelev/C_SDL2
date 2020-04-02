#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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

SDL_Point *make_points_cloud(unsigned size, int cx, int cy, int radius) {
	SDL_Point *cloud = malloc(size * sizeof(SDL_Point));
	if (NULL == cloud) return NULL;
	srand(23);
	for (unsigned idx = 0; idx != size; ++idx) {
		int x_rand = cx + rand() % (2 * radius) - radius;
		int y_rand = cy + rand() % (2 * radius) - radius;
		cloud[idx] = (SDL_Point){ x_rand, y_rand };
	}
	return cloud;
}

void destroy_points_cloud(SDL_Point *cloud) {
	free(cloud);
}

int main(int agc, char **argv) {
	if (0 != SDL_Init(SDL_INIT_VIDEO)) {
		printf("Can't initialize SDL.\nCause: %s\n", SDL_GetError());
		goto END;
	}

	//Основная программа
	{
		SDL_Window *main_window = NULL;
		SDL_Renderer *main_renderer = NULL;
		SDL_Point *cloud = NULL;

		int const SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600, CLOUD_SIZE = 300;
		const char *error_message = "";

		if (*(error_message = init_window_and_renderer(&main_window, &main_renderer, SCREEN_WIDTH, SCREEN_HEIGHT, "SDL Render Draw Example"))) {
			printf("Can't create widnow or renderer.\nCause: %s\n", error_message);
			goto CLEANUP;
		}

		//Создаём набор случайных точек для рисования
		if (NULL == (cloud = make_points_cloud(CLOUD_SIZE, 400, 300, 30))) {
			printf("Can't create a cloud of points.\n");
			goto CLEANUP;
		}

		//Цикл обработки сообщений
		SDL_Event e; //переменная, в которую читаем сообщение
		bool proceed = true;
		while (proceed) {
			if (SDL_PollEvent(&e)) {
				switch (e.type) {
					case SDL_QUIT: {
						proceed = false;
						break;
					}
				}
			}

			//Здесь будет происходить всё рисование
			//Очищаем экран
			SDL_SetRenderDrawColor(main_renderer, 0xFF, 0xFF, 0xFF, 0xFF); //сплошной белый цвет
			SDL_RenderClear(main_renderer);

			//Рисуем пару линий
			SDL_SetRenderDrawColor(main_renderer, 0x00, 0x00, 0x00, 0xFF); //чёрный цвет, чтобы линия была видна на белом фоне
			SDL_RenderDrawLine(main_renderer, 0, 0, 800, 600); //рисуем линию по четырём координатам: начало и конец
			SDL_Point line[2] = { {0,600}, {800,0} }; //SDL_Point - структура, моделирующая точку
			SDL_RenderDrawLines(main_renderer, line, 2); //рисуем линию, как элемент ломанной
			
			//Рисуем треугольник как ломаную
			SDL_Point triangle[4] = { {300,250}, {500,250}, {400,450}, {300,250} }; //чтобы нарисовать трёхзвенную ломанную нужно четыре точки
			SDL_SetRenderDrawColor(main_renderer, 0x33, 0x99, 0xFF, 0xFF); //синий цвет, чтобы отличать от линий
			SDL_RenderDrawLines(main_renderer, triangle, 4);

			//Рисуем прямоугольник незакрашенный
			SDL_SetRenderDrawColor(main_renderer, 0xFF, 0x99, 0x33, 0xFF); //красный цвет, чтобы отличать от других фигур
			SDL_Rect unfilled_rect = { 280,230,240,240 }; //левый верхний угол, ширина и высота
			SDL_RenderDrawRect(main_renderer, &unfilled_rect); //функция SDL_RenderDrawRect принимает именно адрес!!! структуры

			//Закрашенный прямоугольник
			SDL_SetRenderDrawColor(main_renderer, 0x00, 0xFF, 0x99, 0xFF);
			SDL_Rect filled_rect = { 280,100,240,100 };
			SDL_RenderFillRect(main_renderer, &filled_rect);

			//Рисуем точку в центре
			SDL_SetRenderDrawColor(main_renderer, 0xCC, 0x00, 0xCC, 0xFF);
			SDL_RenderDrawPoint(main_renderer, 400, 50);

			//Рисуем облако точек
			SDL_RenderDrawPoints(main_renderer, cloud, CLOUD_SIZE);

			//показываем, что нарисовали
			SDL_RenderPresent(main_renderer);
		}


	CLEANUP:
		destroy_window_and_renderer(main_window, main_renderer);
		destroy_points_cloud(cloud);
	}

END:
	SDL_Quit();
	return 0;
}