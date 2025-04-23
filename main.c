#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "./vendor/stb/stb_image.h"
#include "./vendor/stb/stb_truetype.h"

#include "./vendor/atlr/atlr.h"


#define CANVAS_DEFAULT_WIDTH 500
#define CANVAS_DEFAULT_HEIGHT 300


typedef struct {
    Vec2* points;
    f64 radius;
    u64 color;
    s64 count;
} CanvasStroke;

int main() {
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("atlr - canvas", CANVAS_DEFAULT_WIDTH, CANVAS_DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(window_surface);
    SDL_Event e;

    // TODO: should I force the pixel format on this surface?
    SDL_Surface* canvas = SDL_CreateSurface(CANVAS_DEFAULT_WIDTH, CANVAS_DEFAULT_HEIGHT, window_surface->format);

    b32 drawing = 0;
    b32 running = 1;

    AtlrArena points_memory = atlr_mem_create_arena(5 * ATLR_MEGABYTE);

    CanvasStroke stroke = {
        .points = points_memory.data,
        .radius = 1.0f,
        .color = 0xFFFF00FF,
        .count = 0,
    };
    while (running) {
        SDL_ClearSurface(canvas, 0, 0, 0, 0);

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

                case SDL_EVENT_MOUSE_MOTION: {
                    if (drawing) {
                        stroke.count++;
                        Vec2* point = (Vec2*)atlr_mem_allocate(&points_memory, sizeof(Vec2));
                        point->x = e.motion.x - (CANVAS_DEFAULT_WIDTH / 2);
                        point->y = (CANVAS_DEFAULT_HEIGHT) - e.motion.y - (CANVAS_DEFAULT_HEIGHT / 2);
                    }
                } break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    stroke.count++;
                    Vec2* point = (Vec2*)atlr_mem_allocate(&points_memory, sizeof(Vec2));
                    point->x = e.motion.x - (CANVAS_DEFAULT_WIDTH / 2);
                    point->y = (CANVAS_DEFAULT_HEIGHT) - e.motion.y - (CANVAS_DEFAULT_HEIGHT / 2);
                    drawing = 1;
                } break;
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    // TODO: create new stroke
                    drawing = 0;
                } break;
                default: break;
            }
        }

        for (s64 i = 0; i < stroke.count - 1; i++) {
            Vec2* p_a = (Vec2*) stroke.points + i;
            Vec2* p_b = (Vec2*) stroke.points + i + 1;
            atlr_rtzr_draw_line(canvas->pixels, CANVAS_DEFAULT_WIDTH, CANVAS_DEFAULT_HEIGHT, *p_a, *p_b, stroke.color);
        }
        
        if (!SDL_BlitSurface(canvas, NULL, window_surface, NULL)) {
            atlr_log_error("failed blit of canvas into window surface");
        }
        SDL_UpdateWindowSurface(window);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    atlr_mem_free(&points_memory, "memory");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
