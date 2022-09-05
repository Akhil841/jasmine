#include "window.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
SDL_Surface* screen;

const unsigned long tile_colors[] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void ui_init()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
}

void ui_event_handle(CPU* cpu)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0)
    {
        //joypad, etc
        //kill emulator if screen closed
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
            cpu->die = true;
    }
}