#pragma once
#include <SDL2/SDL.h> 
#include "CPU.h"

const int WIDTH = 160;
const int HEIGHT = 144;

void ui_init();
void ui_event_handle(CPU* cpu);