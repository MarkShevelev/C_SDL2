#include "sdl_aux.h"

#include <SDL.h>

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

void IKI_DrawCircle(SDL_Renderer *renderer, IKI_Circle const *circle) {
	for (int h = 0; h < circle->r; ++h) {
		int x_offset = (int)sqrt(circle->r * circle->r - h * h);
		SDL_RenderDrawLine(renderer, circle->cx - x_offset, circle->cy - h, circle->cx + x_offset, circle->cy - h);
		SDL_RenderDrawLine(renderer, circle->cx - x_offset, circle->cy + h, circle->cx + x_offset, circle->cy + h);
	}
}

void IKI_DrawSpring(SDL_Renderer *renderer, IKI_Spring const *spring) {
	SDL_Point prev = spring->attach, next = spring->attach;
	int x_offset = spring->length / (spring->leg_count - 1);
	next.x += x_offset / 2;

	int y_offset = (spring->leg_length * spring->leg_count - spring->length) / spring->leg_count;
	next.y = y_offset + spring->attach.y;

	for (unsigned cnt = 0; cnt != spring->leg_count - 1; ++cnt) {
		SDL_RenderDrawLine(renderer, prev.x, prev.y, next.x, next.y);
		prev = next;
		next.x += x_offset;
		next.y = (cnt % 2 ? y_offset : -y_offset) + spring->attach.y;
	}

	next.x = spring->attach.x + spring->length;
	next.y = spring->attach.y;
	SDL_RenderDrawLine(renderer, prev.x, prev.y, next.x, next.y);
}