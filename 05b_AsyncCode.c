#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    int const SCREEN_WIDTH = 800;
    int const SCREEN_HEIGHT = 600;

    SDL_ClearError();
    SDL_Window *window_ptr = SDL_CreateWindow(
        "SDL2 Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
        );

    {
        char const *error_message = SDL_GetError();
        if (*error_message) {
            printf("Window construction failed!\nCause: %s\n", error_message);
            goto END;
        }
    }

    //SDL_Delay(3000);
    SDL_Event e;
    bool proceed = true;
    while (proceed) {
        if (SDL_PollEvent(&e)) { //SDL_WaitEvent - ждёт сообщения. SDL_WaitEventTimeout - ждёт сообщение огранич. время
            switch (e.type) {
                case SDL_QUIT: {
                    printf("Quit event\n");
                    proceed = false;
                    break;
                }
            }
        }
        //Иные действия
    }

CLEAR:
    SDL_DestroyWindow(window_ptr);

END:
    SDL_Quit();

	return 0;
}