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

    {
        SDL_Window *main_window = NULL;
        SDL_Renderer *main_window_renderer = NULL;

        SDL_ClearError();
        main_window = SDL_CreateWindow(
            "SDL2 Demo",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
            );

        {
            char const *error_message = SDL_GetError();
            if (*error_message) {
                printf("Window construction failed!\nCause: %s\n", error_message);
                goto CLEANUP;
            }
        }

        SDL_ClearError();
        main_window_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        {
        char const *error_message = SDL_GetError();
            if (*error_message) {
                printf("Renderer construction failed!\nCause: %s\n", error_message);
                goto CLEANUP;
            }
        }

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
            
            //draw
            SDL_SetRenderDrawColor(main_window_renderer, 0x33, 0x99, 0xFF, 0xFF);
            SDL_RenderClear(main_window_renderer);
            SDL_RenderPresent(main_window_renderer);
        }

    CLEAR:
        if (NULL == main_window_renderer) SDL_DestroyRenderer(main_window_renderer);
        if (NULL == main_window) SDL_DestroyWindow(main_window);

    }

    
END:
    SDL_Quit();

	return 0;
}