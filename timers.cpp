#include "timers.h"
#include <iostream>

Timers::Timers(CPU* cpu)
{
    this->cpu = cpu;
    tick = 0;
}

int Timers::TIMA_T(int selection)
{
    if (selection < 0 || selection > 3)
    {
        std::cout << "TIMERS: TAC bits 1-0 cannot be greater than 3 or less than 0!" << std::endl;
        throw 1;
    }
    int outs[] = { T_1024, T_16, T_64, T_256 };
    return outs[selection];
}

void Timers::step()
{
    //check timer settings
    unsigned char curtac = cpu->mmu.r8(0xFF07);
    //do nothing if disabled
    if (!(curtac & 0x4))
        return;
    //get threshold
    int threshold = TIMA_T(curtac & 0x3);
    //increment tick
    tick++;
    //increment DIV using T_16
    if (tick % T_16 == 0)
    {
        unsigned char curdiv = cpu->mmu.r8(0xFF04);
        curdiv++;
        cpu->mmu.w8(curdiv, 0xFF04);
    }
    //increment TIMA using selected threshold
    if (tick % threshold == 0)
    {
        unsigned char curtima = cpu->mmu.r8(0xFF05);
        curtima++;
        if (curtima)
        {
            cpu->mmu.w8(curtima, 0xFF05);
            return;
        }
        else
        {
            //reset TIMA to TMA
            cpu->mmu.w8(cpu->mmu.r8(0xFF06), 0xFF05);
            //send timer interrupt
            cpu->request_interrupt(TIMER);
        }
    }

}

void Timers::step(int ticks)
{
    for (int i = 0; i < ticks; i++)
        step();
}