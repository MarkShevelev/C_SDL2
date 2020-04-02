#include <SDL.h>
#include <stdio.h>

int main(int arc, char **argv) {
    if (0 != SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SDL_ClearError(); //Сбрасываем предыдущую ошибку
    SDL_Window *window = SDL_CreateWindow(
        "SDL2 Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
        );

    {//Блок обработки ошибки
        char const *error_message = SDL_GetError(); //Получаем сообщение об ошибке
        if (*error_message) { //Если сообщение не пусто, то возникла ошибка
            printf("Window construction failed!\nCause: %s\n",error_message);
            goto END;
        }
    }

    SDL_Delay(3000);

    SDL_DestroyWindow(window);

END:
    SDL_Quit();

    return 0;
}