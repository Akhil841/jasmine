#include <iostream>   
#include "CPU.h"
#include "PPU.h"
#include "timers.h"
#include "window.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Error: no input files" << std::endl;
        return 1;
    }
    std::string filepath = std::string(*(argv + 1));
    CPU cpu(filepath);
    cpu.mmu.load_bios();
    PPU ppu(&cpu);
    Timers timers(&cpu);
    ui_init();
    unsigned int prevframe = 0;
    //bool fuse = true;
    while (cpu.pc < 0xA00 && !cpu.die)
    {
        // bios "jailbreaks"
        // if (cpu.pc == 0x66 && fuse)
        //     cpu.a = 0x90;
        // if (cpu.pc == 0x8C && fuse)
        //     cpu.f |= 0x80;
        if (cpu.pc == 0x100)
        {
            cpu.mmu.load_rom(filepath);
        //  fuse = false;
        }
        cpu.step();
        std::cout << "t = 0x" << std::hex << timers.tick << std::dec << std::endl;
        timers.step(cpu.m * 4);
        ppu.step();
        ui_event_handle(&cpu);
        if (prevframe != ppu.currentframe)
        {
            //ui_update(&ppu);
        }
        prevframe = ppu.currentframe;
    }
    return 0;
}