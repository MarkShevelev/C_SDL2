#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

typedef struct _IKI_Circle {
	int cx, cy, r;
} IKI_Circle;

void IKI_DrawCircle(SDL_Renderer *renderer, IKI_Circle const *circle) {
	for (int h = 0; h < circle->r; ++h) {
		int x_offset = (int)sqrt(circle->r * circle->r - h * h);
		SDL_RenderDrawLine(renderer, circle->cx - x_offset, circle->cy - h, circle->cx + x_offset, circle->cy - h);
		SDL_RenderDrawLine(renderer, circle->cx - x_offset, circle->cy + h, circle->cx + x_offset, circle->cy + h);
	}
}

void destroy_window_and_renderer(SDL_Window *window, SDL_Renderer *renderer) {
	if (NULL != renderer) SDL_DestroyRenderer(renderer);
	if (NULL != window) SDL_DestroyWindow(window);
}

typedef struct _IKI_Scene {
	IKI_Circle *circle;
} IKI_Scene;

void draw_scene(SDL_Renderer *renderer, IKI_Scene const *scene) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0xCC, 0x33, 0x33, 0xFF);
	IKI_DrawCircle(renderer, scene->circle);

	SDL_RenderPresent(renderer);
}

Uint32 timer_function(Uint32 interval, void *_params) {
	SDL_UserEvent userevent;  
	userevent.type = SDL_USEREVENT;  
	userevent.code = 0;

	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user = userevent; 

	SDL_PushEvent(&event);
	return interval;
}

typedef struct _IKI_Model {
	double x, y, vx, vy, ax, ay;
} IKI_Model;

void map_model_to_scene(IKI_Scene *scene, IKI_Model const *model) {
	scene->circle->cx = 50 + model->x * 700;
	scene->circle->cy = 600 - (50 + model->y * 500);
}

void model_time_step(IKI_Model *model, double dt) {
	model->x += model->vx * dt;
	model->y += model->vy * dt;

	model->vx += model->ax * dt;
	model->vy += model->ay * dt;
	if (model->vx > 2. || model->vx < -2.) model->vx = model->vx > 0 ? 1. : -1.;
	if (model->vy > 2. || model->vy < -2.) model->vy = model->vy > 0 ? 1. : -1.;

	if (model->x > 1. || model->x < 0.) model->vx *= -1.;
	if (model->y > 1. || model->y < 0.) model->vy *= -1.;
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
		SDL_TimerID draw_timer_id = 0;

		int const SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600, CLOUD_SIZE = 300;
		const char *error_message = "";

		if (*(error_message = init_window_and_renderer(&main_window, &main_renderer, SCREEN_WIDTH, SCREEN_HEIGHT, "SDL Render Draw Example"))) {
			printf("Can't create widnow or renderer.\nCause: %s\n", error_message);
			goto CLEANUP;
		}

		IKI_Circle moving_circle = { 400, 300, 50 };
		IKI_Scene scene = { &moving_circle };
		IKI_Model model = { 0.5, 0.5, 0., 0., 0., 0. };
		bool moving = true;
		double accel = 2.0;

		draw_timer_id = SDL_AddTimer(33, timer_function, NULL);

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
						map_model_to_scene(&scene, &model);
						draw_scene(main_renderer,&scene);
						break;
					}

					case SDL_KEYDOWN:                       
					{
						SDL_KeyboardEvent key = e.key;
						switch (key.keysym.sym) {
							case SDLK_DOWN:
								model.ay = -accel;
								break;

							case SDLK_UP:
								model.ay = accel;
								break;

							case SDLK_LEFT:
								model.ax = -accel;
								break;

							case SDLK_RIGHT:
								model.ax = accel;
								break;

							case SDLK_SPACE:
								moving = !moving;
								break;
						}
						break;
					}

					case SDL_KEYUP:
					{
						SDL_KeyboardEvent key = e.key;
						switch (key.keysym.sym) {
							case SDLK_DOWN:
							case SDLK_UP:
								model.ay = 0.;
								break;

							case SDLK_LEFT:
							case SDLK_RIGHT:
								model.ax = 0.;
								break;
						}
						break;
					}
				}
			}

			if (moving) 
				model_time_step(&model, 2.e-6);
		}


	CLEANUP:
		destroy_window_and_renderer(main_window, main_renderer);
		if (0 != draw_timer_id) { SDL_RemoveTimer(draw_timer_id); }
	}

END:
	SDL_Quit();
	return 0;
}