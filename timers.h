#pragma once
#include "CPU.h"

class Timers
{
    CPU* cpu;
    public:
        Timers(CPU* cpu);
        void step();
        void step(int ticks);
        unsigned long tick;
    private:
        //get required threshold
        int TIMA_T(int selection);
        //clock 0 = clock / 1024
        const int T_1024 = 1024;
        //clock 1 = clock / 16
        const int T_16 = 16;
        //clock 2 = clock / 64
        const int T_64 = 64;
        //clock 3 = clock / 256
        const int T_256 = 256;
};