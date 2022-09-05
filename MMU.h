#pragma once
#include <string>
enum MMUType
{
    NONE,
    MBC1,
    MBC2,
    MBC3,
    MBC5,
    UNSUPPORTED
};

class MMU
{
    public:
        MMU();
        MMU(std::string & rompath);
        void init(std::string rompath);
        unsigned char r8(unsigned short location);
        unsigned short r16(unsigned short location);
        void w8(unsigned char value, unsigned short location);
        void w16(unsigned short value, unsigned short location);
        void free();
        void load_bios();
        void load_rom(std::string filepath);
    private:
        unsigned char* rom;
        int romsize;
        unsigned char* rombanks;
        unsigned char* vram;
        unsigned char* sram;
        bool sramenabled;
        unsigned char* srambank;
        //the number of the bank loaded in 0xA000 - 0xBFFF
        char srambanknum;
        //the number of banks in SRAM
        unsigned char sramnumbanks;
        unsigned char* wram;
        unsigned char* oam;
        unsigned char* badmem;
        unsigned char* ioram;
        unsigned char* zram;
        MMUType detMMU(unsigned char mmcode);
        unsigned char sramsize(unsigned char ramcode);
        MMUType mmukind;
        char mbc1mode;
        unsigned char mbc5bits18;
        char mbc5bit9;
};