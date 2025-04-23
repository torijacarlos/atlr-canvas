#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "./vendor/stb/stb_image.h"
#include "./vendor/stb/stb_truetype.h"

#include "./vendor/atlr/atlr.h"


#define CANVAS_DEFAULT_WIDTH 500
#define CANVAS_DEFAULT_HEIGHT 300

int main() {
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("atlr - canvas", CANVAS_DEFAULT_WIDTH, CANVAS_DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(window_surface);

    b32 running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT: {
                    running = 0;
                } break;
                case SDL_EVENT_KEY_UP: {
                    if (e.key.key == 'q') {
                        running = 0;
                    }
                } break;
                default: break;
            }
        }
        
        SDL_UpdateWindowSurface(window);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
