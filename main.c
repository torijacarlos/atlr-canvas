#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "./vendor/stb/stb_image.h"
#include "./vendor/stb/stb_truetype.h"

#include "./vendor/atlr/atlr.h"


#define CANVAS_DEFAULT_WIDTH 500
#define CANVAS_DEFAULT_HEIGHT 300


// TODO: Handle canvas resizing
// TODO: Enable stroke size
// TODO: Enable color picking

typedef struct {
    Vec2 origin;
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
    b32 moving = 0;
    b32 running = 1;

    AtlrArena main_memory = atlr_mem_create_arena(5 * ATLR_MEGABYTE);
    AtlrArena strokes_memory = atlr_mem_slice(&main_memory, 1 * ATLR_MEGABYTE);
    AtlrArena points_memory = atlr_mem_slice(&main_memory, 1 * ATLR_MEGABYTE);
    AtlrArena font_memory = atlr_mem_slice(&main_memory, 1 * ATLR_MEGABYTE);

    CanvasStroke* strokes = (CanvasStroke*) strokes_memory.data;
    s64 strokes_count = 0;
    CanvasStroke* stroke;
    AtlrString color_label = atlr_str_create_empty_with_capacity(15);
    u64 color = 0xFFFF00FF;

    AtlrFont nunito_font = atlr_font_load("./static/nunito.ttf", 24, &font_memory);
    Vec2 click_origin = {};
    while (running) {
        SDL_ClearSurface(canvas, 0.07f, 0.07f, 0.07f, 1.0f);
        atlr_str_clear(&color_label);

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
                        Vec2* point = (Vec2*)atlr_mem_allocate(&points_memory, sizeof(Vec2));
                        point->x = e.motion.x - (canvas->w / 2) - stroke->origin.x;
                        point->y = (canvas->h) - e.motion.y - (canvas->h / 2) - stroke->origin.y;
                        stroke->count++;
                    } else if (moving) {
                        for (s64 s = 0; s < strokes_count; s++) {
                            CanvasStroke* curr_stroke = (CanvasStroke*) strokes_memory.data + s;
                            curr_stroke->origin.x += e.motion.x - (canvas->w / 2) - click_origin.x;
                            curr_stroke->origin.y += (canvas->h) - e.motion.y - (canvas->h / 2) -  click_origin.y;
                        }
                        click_origin.x = e.motion.x - (canvas->w / 2);
                        click_origin.y = (canvas->h) - e.motion.y - (canvas->h / 2);
                    }
                } break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    atlr_log_debug("button: %d", e.button.button);
                    if (e.button.button == 1) {
                        stroke = (CanvasStroke*) atlr_mem_allocate(&strokes_memory, sizeof(CanvasStroke));
                        stroke->origin.x = e.button.x - (canvas->w / 2);
                        stroke->origin.y = (canvas->h) - e.button.y - (canvas->h / 2);
                        stroke->points = (Vec2*) ((u8*) points_memory.data + points_memory.used);
                        stroke->radius = 1.0f;
                        stroke->color = color;
                        stroke->count = 0;
                        strokes_count++;

                        Vec2* point = (Vec2*)atlr_mem_allocate(&points_memory, sizeof(Vec2));
                        point->x = e.button.x - (canvas->w / 2) - stroke->origin.x;
                        point->y = (canvas->h) - e.button.y - (canvas->h / 2) - stroke->origin.y;
                        drawing = 1;
                        stroke->count++;
                    } else if (e.button.button == 2) {
                        moving = 1;
                        click_origin.x = e.button.x - (canvas->w / 2);
                        click_origin.y = (canvas->h) - e.button.y - (canvas->h / 2);
                    }
                
                } break;
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    if (e.button.button == 1) {
                        drawing = 0;
                    } else if (e.button.button == 2) {
                        moving = 0;
                    }
                } break;
                case SDL_EVENT_MOUSE_WHEEL: {
                    u64 r = ((color & 0xFF000000) >> 24) + (e.wheel.y + 10);
                    u64 g = ((color & 0x00FF0000) >> 16) + (e.wheel.y + 10);
                    u64 b = ((color & 0x0000FF00) >> 8) +  (e.wheel.y + 10);
                    u64 a = ((color & 0x000000FF));
                    color = (r << 24 | g << 16 | b << 8 | a);
                } break;

                case SDL_EVENT_WINDOW_RESIZED: {
                    window_surface = SDL_GetWindowSurface(window);
                    SDL_DestroySurface(canvas);
                    canvas = SDL_CreateSurface(window_surface->w, window_surface->h, window_surface->format);
                } break;
                default: break;
            }
        }

        for (s64 s = 0; s < strokes_count; s++) {
            CanvasStroke* curr_stroke = (CanvasStroke*) strokes_memory.data + s;
            for (s64 i = 0; i < curr_stroke->count - 1; i++) {
                Vec2* p_a = (Vec2*) curr_stroke->points + i;
                Vec2* p_b = (Vec2*) curr_stroke->points + i + 1;
                Vec2 pa = {
                    .x = curr_stroke->origin.x + p_a->x,
                    .y = curr_stroke->origin.y + p_a->y,
                };
                Vec2 pb = {
                    .x = curr_stroke->origin.x + p_b->x,
                    .y = curr_stroke->origin.y + p_b->y,
                };
                atlr_rtzr_draw_line(canvas->pixels, canvas->w, canvas->h, pa, pb, curr_stroke->color);
            }
        }

        sprintf(color_label.data, "0x%08lX", color);
        color_label.len = strlen(color_label.data);

        atlr_rtzr_draw_label(
            canvas->pixels, 
            canvas->w, 
            canvas->h, 
            &color_label, 
            0xFFFFFFFF, 
            (Vec2) { .x = - (canvas->w / 2), .y = (canvas->h / 2)}, 
            &nunito_font
        );
        if (!SDL_BlitSurface(canvas, NULL, window_surface, NULL)) {
            atlr_log_error("failed blit of canvas into window surface");
        }
        SDL_UpdateWindowSurface(window);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    atlr_mem_clear(&points_memory, "points");
    atlr_mem_clear(&strokes_memory, "strokes");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
