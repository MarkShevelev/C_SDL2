#include "sdl_aux.h"

#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Uint32 timer_function_draw_event(Uint32 interval, void *_params) {
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;

	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return interval;
}

typedef struct {
	IKI_Circle *circle;
	IKI_Spring *spring;
} IKI_Scene;

void draw_scene(SDL_Renderer *renderer, IKI_Scene const *scene) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0x99, 0x33, 0xCC, 0xFF);
	IKI_DrawCircle(renderer, scene->circle);

	SDL_SetRenderDrawColor(renderer, 0xCFF, 0x33, 0x99, 0xFF);
	IKI_DrawSpring(renderer, scene->spring);

	SDL_RenderPresent(renderer);
}

//Структура IKI_Model описывает данные, необходимые для моделирования движения тела на пружине
typedef struct {
	double k; //линейная жёсткость; F = -k * dx; dx - отклонение от положения равновесия
	double m; //масса тела;
	double dx,v; //dx - смещение; dx = 0 - положение равновесия; v - мгновенная скорость
} IKI_Model;

//Шаг по времени по схеме Эйлера
void model_euler_time_step(IKI_Model *model, float dt) {
	model->dx += model->v * dt; 
	model->v += -model->k * model->dx / model->m * dt; 
}

//Шаг по времени по схеме 'предиктор-корректор'
void model_predictor_time_step(IKI_Model *model, float dt) {
	double accel = -model->k * model->dx / model->m; //ускорение в момент времени t
	double v_predict = model->v + accel * dt * 0.5;  //скорость в момент времени t + 0.5dt - 'предсказанная' по текущему ускорению скорость
	model->dx += v_predict * dt;                     //новое значение смещения, учитывающее ускорение

	double accel_predict = -model->k * model->dx / model->m; //новое, 'предсказанное' значение ускорения
	model->v += (accel + accel_predict) * 0.5 * dt;          //на основе среднего ускорения вычисляем новую скорость
}

//Функция преобразования из координат модели в координаты окна
void map_model_to_scene(IKI_Model const *model, IKI_Scene *scene) {
	int circle_position = model->dx * 350 + 400;
	scene->circle->cx = circle_position;
	scene->spring->length = circle_position;
}

//Функция для вывода на экран полной энергии системы
void log_energy(IKI_Model const *model) {
	printf("E=%f\n", model->m * model->v * model->v / 2. + model->k * model->dx * model->dx / 2.);
}

int main(int agc, char **argv) {
	if (0 != SDL_Init(SDL_INIT_VIDEO)) {
		printf("Can't initialize SDL.\nCause: %s\n", SDL_GetError());
		goto END;
	}

	{
		SDL_Window *main_window = NULL;
		SDL_Renderer *main_renderer = NULL;
		SDL_TimerID timer_id;

		int const SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
		const char *error_message = "";

		if (*(error_message = init_window_and_renderer(&main_window, &main_renderer, SCREEN_WIDTH, SCREEN_HEIGHT, "SDL Render Draw Example"))) {
			printf("Can't create widnow or renderer.\nCause: %s\n", error_message);
			goto CLEANUP;
		}

		IKI_Model model = { 0.1, 4., 0., 0.1 }; double dt = 2.5e-5; //Данные модели (model) и шаг по времени (dt)

		IKI_Circle circle = { 400, 300, 50 };
		IKI_Spring spring = { (SDL_Point) { 0,300 }, 400, 50, 20 };
		IKI_Scene scene = { &circle, &spring };

		timer_id = SDL_AddTimer(33, timer_function_draw_event, NULL);

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
						//log_energy(&model); //раскомментировать для вывода на экран полной энергии
						map_model_to_scene(&model, &scene);
						draw_scene(main_renderer, &scene);
						break;
					}
				}
			}

			//model_euler_time_step(&model,dt);       //раскомментировать для вычисления движения тела по схеме Эйлера
			//model_predictor_time_step(&model, dt);  //раскомментировать для вычисления движения тела по схеме 'предиктор-корректор'
			
		}

	CLEANUP:
		destroy_window_and_renderer(main_window, main_renderer);
		SDL_RemoveTimer(timer_id);
	}

END:
	SDL_Quit();
	return 0;
}