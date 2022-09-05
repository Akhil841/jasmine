#pragma once
#include "CPU.h"
#include <chrono>
#include <thread>

enum PPU_state
{
    HBLANK, //mode 0
    VBLANK, //mode 1
    OAM,    //mode 2
    TRANSFER//mode 3
};

enum FIFO_state
{
    TILE,
    DATA0,
    DATA1,
    SLEEP,
    PUSH
};

class FIFOpage
{
    public:
        FIFOpage* next;
        unsigned int val; 
        FIFOpage(unsigned int val);
};

class FIFO
{
    public:
        FIFO_state state;
        unsigned char line_x;
        unsigned char pushed_x;
        unsigned char fetch_x;
        unsigned char bgw_fetch_data[3];
        unsigned char fetch_entry_data[6];
        unsigned char map_x;
        unsigned char map_y;
        unsigned char tile_x;
        unsigned char tile_y;
        unsigned char fifo_x;
        FIFO();
        FIFOpage* head;
        FIFOpage* tail;
        unsigned char size;
        FIFOpage* pop();
        void push(FIFOpage* page);
        void reset();
        void fetch();
};

class PPU
{
    public:
        PPU(CPU* cpu);
        void init();
        void step();
        void lcd_init();
        void free();
        PPU_state mode;
        unsigned int* framebuf;
        unsigned int currentframe;
        void pipeline_process();
    private:
        void push_pixel();
        CPU* cpu;
        const int BGW = 7;
        const int YFLIP = 6;
        const int XFLIP = 5;
        const int PALNUM = 4;
        FIFO fifo;  
        const unsigned short LCDC_ADDR = 0xFF40;
        const unsigned short LCDS_ADDR = 0xFF41;
        const unsigned short SCY_ADDR = 0xFF42;
        const unsigned short SCX_ADDR = 0xFF43;
        const unsigned short LY_ADDR = 0xFF44;
        const unsigned short LYC_ADDR = 0xFF45;
        const unsigned short BGP_ADDR = 0xFF47;
        const unsigned short OBP0_ADDR = 0xFF48;
        const unsigned short OBP1_ADDR = 0xFF49;
        const unsigned short WY_ADDR = 0xFF4A;
        const unsigned short WX_ADDR = 0xFF4B;
        const int LINES_PER_FRAME = 154;
        const int TICKS_PER_LINE = 456;
        const int HEIGHT = 144;
        const int WIDTH = 160;
        unsigned int lineticks;
        void oam();
        void transfer();
        void hblank();
        void vblank();
        void incy();
};