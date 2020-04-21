#pragma once

#include <SDL.h>

char const *init_window_and_renderer(SDL_Window **window_indirect, SDL_Renderer **renderer_indirect, int width, int height, char const *ascii_title);
void destroy_window_and_renderer(SDL_Window *window, SDL_Renderer *renderer);

typedef struct {
	int cx, cy, r;
} IKI_Circle;

void IKI_DrawCircle(SDL_Renderer *renderer, IKI_Circle const *circle);

typedef struct {
	SDL_Point attach;
	int length, leg_length;
	unsigned leg_count;
} IKI_Spring;

void IKI_DrawSpring(SDL_Renderer *renderer, IKI_Spring const *spring);