#include "PPU.h"

const unsigned int tile_colors[] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

PPU::PPU(CPU* cpu)
{
    this->cpu = cpu;
    init();
}

void PPU::init()
{
    fifo = FIFO();
    currentframe = 0;
    lineticks = 0;
    framebuf = new unsigned int[WIDTH * HEIGHT];
    lcd_init();
    mode = OAM;
    for (int i = 0; i < 0xA0; i++)
    {
        cpu->mmu.w8(0, 0xFE00 + i);
    }
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        framebuf[i] = 0;
    }
}

void PPU::lcd_init()
{
    //initialize values
    cpu->mmu.w8(0x91, LCDC_ADDR);
    cpu->mmu.w8(0x00, SCX_ADDR);
    cpu->mmu.w8(0x00, SCY_ADDR);
    cpu->mmu.w8(0x00, LY_ADDR);
    cpu->mmu.w8(0x00, LYC_ADDR);
    cpu->mmu.w8(0xFC, BGP_ADDR);
    cpu->mmu.w8(0xFF, OBP0_ADDR);
    cpu->mmu.w8(0xFF, OBP1_ADDR);
    cpu->mmu.w8(0x00, WX_ADDR);
    cpu->mmu.w8(0x00, WY_ADDR);
}

void PPU::step()
{
    lineticks++;
    unsigned char lcds = cpu->mmu.r8(LCDS_ADDR) & 0x03;
    switch (lcds)
    {
        case 0x00:
            mode = HBLANK;
            hblank();
            break;
        case 0x01:
            mode = VBLANK;
            vblank();
            break;
        case 0x02:
            mode = OAM;
            oam();
            break;
        case 0x03:
            mode = TRANSFER;
            transfer();
            break;
    }
}

void PPU::hblank()
{
    if (lineticks >= TICKS_PER_LINE)
    {
        incy();
        if (cpu->mmu.r8(LY_ADDR) >= HEIGHT)
        {
            mode = VBLANK;
            cpu->mmu.w8((cpu->mmu.r8(LCDS_ADDR) & 0xFC) + 0x01, LCDS_ADDR);
            cpu->request_interrupt(VB);
            if (((cpu->mmu.r8(LCDS_ADDR) & 0b00010000) >> 6) == 0x1)
                cpu->request_interrupt(LCDSTAT);
            currentframe++;
            //sync time (kind of)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
        }
        else
        {
            mode = OAM;
            cpu->mmu.w8((cpu->mmu.r8(LCDS_ADDR) & 0xFC) + 0x02, LCDS_ADDR);
        }
        lineticks = 0;
    }
}

void PPU::vblank()
{
    if (lineticks >= TICKS_PER_LINE)
    {
        incy();
        if (cpu->mmu.r8(LY_ADDR) >= LINES_PER_FRAME)
        {
            mode = OAM;
            cpu->mmu.w8((cpu->mmu.r8(LCDS_ADDR) & 0xFC) + 0x02, LCDS_ADDR);
            cpu->mmu.w8(0, LY_ADDR);
        }
        lineticks = 0;  
    }
}

void PPU::oam()
{
    if (lineticks >= 80)
    {
        mode = TRANSFER;
        cpu->mmu.w8((cpu->mmu.r8(LCDS_ADDR) & 0xFC) + 0x03, LCDS_ADDR);
        fifo.state = TILE;
        fifo.line_x = 0;
        fifo.fetch_x = 0;
        fifo.pushed_x = 0;
        fifo.fifo_x = 0;
    }
}

void PPU::transfer()
{
    pipeline_process();
    if (fifo.pushed_x >= WIDTH)
    {
        fifo.reset();
        mode = HBLANK;
        cpu->mmu.w8((cpu->mmu.r8(LCDS_ADDR) & 0xFC) + 0x00, LCDS_ADDR);
        if (((cpu->mmu.r8(LCDS_ADDR) & 0b00010000) >> 4) == 0x1)
            cpu->request_interrupt(LCDSTAT);
    }
}

void PPU::free()
{
    delete[] framebuf;
}

void PPU::incy()
{
    cpu->mmu.w8(cpu->mmu.r8(LY_ADDR) + 1, LY_ADDR);
    if (cpu->mmu.r8(LY_ADDR) == cpu->mmu.r8(LYC_ADDR))
    {
        cpu->mmu.w8(cpu->mmu.r8(LCDS_ADDR) | (1 << 6), LCDS_ADDR);
        if (((cpu->mmu.r8(LCDS_ADDR) & 0b01000000) >> 6) == 0x1)
            cpu->request_interrupt(LCDSTAT);
    }
    else
        cpu->mmu.w8(cpu->mmu.r8(LCDS_ADDR) & ~(1 << 6), LCDS_ADDR);
}

FIFOpage::FIFOpage(unsigned int val)
{
    this->val = val;
    next = nullptr;
}

FIFO::FIFO()
{
    line_x = 0;
    pushed_x = 0;
    fetch_x = 0;
    state = TILE;
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void FIFO::push(FIFOpage* page)
{
    if (size == 0)
    {
        head = page;
        tail = page;
    }
    else
    {
        tail->next = page;
        tail = page;
    }
    size++;
}

FIFOpage* FIFO::pop()
{
    if (size == 0)
        return nullptr;
    if (size == 1)
    {
        FIFOpage* out = head;
        head = nullptr;
        tail = nullptr;
        return out;
    }
    FIFOpage* out = head;
    head = head->next;

    return out;
}

void FIFO::reset()
{

}

void PPU::push_pixel()
{
    if (fifo.size > 8)
    {
        FIFOpage* out = fifo.pop();
        unsigned int data = out->val;
        delete out;
        // if (line_x >= )
    }
}

void PPU::pipeline_process()
{
    fifo.map_y = cpu->mmu.r8(LY_ADDR) + cpu->mmu.r8(SCY_ADDR);
    fifo.map_x = fifo.fetch_x + cpu->mmu.r8(SCX_ADDR);
    fifo.tile_y = ((cpu->mmu.r8(LY_ADDR) + cpu->mmu.r8(SCY_ADDR)) % 8) * 2;
    if (!(lineticks & 1))
    {
        //fifo.fetch();
    }
    // fifo.push_pixel();
}