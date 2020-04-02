#include <SDL.h>
#include <stdio.h>

int main(int arc, char **argv) {
    if (0 != SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SDL_CreateWindow(
        "SDL2 Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
        );

    SDL_Event e;
    while (1) {
        if (SDL_PollEvent(&e)) {
            if (SDL_QUIT == e.type) break;
        }
    }

    SDL_Quit();

    return 0;
}