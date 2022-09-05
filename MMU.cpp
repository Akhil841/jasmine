#include <fstream>
#include <iostream>
#include <cmath>
#include "MMU.h"

MMU::MMU()
{
    romsize = 0;
    sramnumbanks = 0;
}

MMU::MMU(std::string & rompath)
{
    romsize = 0;
    sramnumbanks = 0;
    init(rompath);
}

void MMU::init(std::string rompath)
{
    std::ifstream romread = std::ifstream(rompath);
    if (!romread)
    {
        std::cout << "MMU: ROM file is missing or does not exist: " 
            << rompath << std::endl;
        throw 1;
    }
    //16MB buffer, twice as big as the biggest Game Boy rom
    const std::streamsize BUFSZ = 16 * 1024 * 1024;
    //store read characters here
    char* buf = new char[BUFSZ];
    //read the ROM
    romread.read(buf, BUFSZ);
    //update rom size
    if ((romsize = romread.gcount()) == 0)
    {
        std::cout << "MMU: ROM file is empty" << std::endl;
        throw 1;
    }
    //initialize ROM
    rom = new unsigned char[romsize];
    //write characters in buffer to ROM
    for (int i = 0; i < romsize; i++)
        *(rom + i) = *(buf + i);
    //delete buffer as it is no longer needed
    delete[] buf;
    //initialize ROM banks 0 and (1)
    rombanks = new unsigned char[32 * 1024];
    //write first 16KB of ROM to bank 0
    //if the ROM is less than 32KB,
    //it must be the bios
    bool bios = (romsize < 32 * 1024);
    for (int i = 0; i < (bios ? romsize : 16 * 1024); i++)
        *(rombanks + i) = *(rom + i);
    //determine MMU if bootrom is not used
    if (!bios)
        mmukind = detMMU(*(rom + 0x0147));
    if (mmukind == UNSUPPORTED)
    {
        std::cout << "MMU: Unsupported memory mapper!" << std::endl;
        throw 1;
    }
    if (mmukind == MBC1)
        mbc1mode = 0;
    //write the next 16KB of ROM to bank 1
    //if it exists
    if (!bios)
        for (int i = 0; i < 16 * 1024; i++)
            *(rombanks + 16 * 1024 + i) = *(rom + 16 * 1024 + i);
    //initialize all other memory
    //8K of VRAM
    vram = new unsigned char[8 * 1024];
    //Variable amount of SRAM
    sramnumbanks = bios ? 0 : sramsize(*(rom + 0x0149));
    if (sramnumbanks)
    {
        sram = new unsigned char[sramnumbanks * 8 * 1024];
        //the current SRAM bank has 8K of ram if it has ram,
        //otherwise none
        srambank = new unsigned char[8 * 1024];
    }
    sramenabled = false;
    srambanknum = 0;
    //8K of WRAM
    wram = new unsigned char[8 * 1024];
    //160 bytes of GPU OAM
    oam = new unsigned char[160];
    //96 bytes of unusable memory
    badmem = new unsigned char[96];
    //128 bytes of IO control values
    ioram = new unsigned char[128];
    //128 bytes of zero-page ram
    zram = new unsigned char[128];
}

//unsigned character to hex string
std::string uctohs(unsigned char value)
{
    std::string out = "";
    char tp = value / 16;
    if (tp < 10)
        out += std::string(1, char(tp + 48));
    else
        out += std::string(1, char(tp + 55));
    char op = value - tp * 16;
    if (op < 10)
        out += std::string(1, char(op + 48));
    else
        out += std::string(1, char(op + 55));
    return out;
}

//the number of SRAM banks onboard
unsigned char MMU::sramsize(unsigned char ramcode)
{
    switch (ramcode)
    {
        case 0:
            return 0;
        //unused
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 4;
        case 4:
            return 16;
        case 5:
            return 8;
    }
    std::cout << "Bad RAM code. Make sure you have a good dump." << std::endl;
    throw 1;
}

MMUType MMU::detMMU(unsigned char mmcode)
{
    MMUType out = NONE;
    if (mmcode > 0x00 && mmcode < 0x04)
        out = MBC1;
    if (mmcode == 0x05 || mmcode == 0x06)
        out = MBC2;
    if (mmcode > 0x0A && mmcode < 0x0E)
        out = UNSUPPORTED; //MMM01
    if (mmcode > 0x0E && mmcode < 0x14)
        out = UNSUPPORTED; //MBC3
    if (mmcode > 0x18 && mmcode < 0x1F)
        out = MBC5;
    if (mmcode == 0x20)
        out = UNSUPPORTED; //MBC6
    if (mmcode == 0x22)
        out = UNSUPPORTED; //MBC7
    if (mmcode == 0xFC)
        out = UNSUPPORTED; //POCKET CAMERA
    if (mmcode == 0xFD)
        out = UNSUPPORTED; //BANDAI TAMA5
    if (mmcode == 0xFE)
        out = UNSUPPORTED; //HuC3
    if (mmcode == 0xFF)
        out = UNSUPPORTED; //HuC1
    return out;
}

void MMU::load_bios()
{
    std::ifstream biosr("bios/bios.gb");
    char* buf = new char[256];
    biosr.read(buf, 256);
    for (int i = 0; i < biosr.gcount(); i++)
        rombanks[i] = buf[i];
    delete[] buf;
}

void MMU::load_rom(std::string filepath)
{
    std::ifstream romsr(filepath);
    char* buf = new char[256];
    romsr.read(buf, 256);
    for (int i = 0; i < romsr.gcount(); i++)
        rombanks[i] = buf[i];
    delete[] buf;
}

//ONLY use after initializing MMU
void MMU::free()
{
    //Attempt to use free() before initializing
    //MMU or using free() without initializing
    if (!romsize)
    {
        std::cout << "MMU: WARNING: Attempt to use MMU.free() "
            << "without initialized memory; will ignore" << std::endl;
        return;
    }
    //free all allocated memory
    delete[] rom;
    delete[] rombanks;
    delete[] vram;
    if (sramnumbanks)
    {
        delete[] sram;
        delete[] srambank;
    }
    delete[] wram;
    delete[] oam;
    delete[] badmem;
    delete[] ioram;
    delete[] zram;
    romsize = 0;
}

unsigned char MMU::r8(unsigned short location)
{
    switch (location & 0xF000)
    {
        //read from rom banks 0 and 1 respectively
        case 0x0000:
        case 0x1000:
        case 0x2000:
        case 0x3000:
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
        return rombanks[location];
        //read from vram
        case 0x8000:
        case 0x9000:
        return vram[location - 0x8000];
        //read from sram if attached and enabled
        case 0xA000:
        case 0xB000:
        if (sramnumbanks)
        {
            if (sramenabled)
                return srambank[location - 0xA000];
            else
            {
                std::cout << "MMU: WARNING: Attempt to read from disabled SRAM" << std::endl;
                return 0;
            }
        }
        break;
        //read from wram
        case 0xC000:
        case 0xD000:
        return wram[location - 0xC000];
        //0xE000 - 0xFDFF contains an unusuable echo
        //of wram
        case 0xE000:
        std::cout << "MMU: WARNING: Attempted read from unusable memory address " << uctohs(location) << std::endl;
        return wram[location - 0xE000];
        case 0xF000:
        switch (location & 0x0F00)
        {
            //0xE000 - 0xFDFF contains an unusable echo
            //of wram
            case 0x000:
            case 0x100:
            case 0x200:
            case 0x300:
            case 0x400:
            case 0x500:
            case 0x600:
            case 0x700:
            case 0x800:
            case 0x900:
            case 0xA00:
            case 0xB00:
            case 0xC00:
            case 0xD00:
            std::cout << "MMU: WARNING: Attempted read from unusable memory address " << uctohs(location) << std::endl;
            return wram[location - 0xE000];
            //Graphics sprite info in 0xFE00 - 0xFE9F
            case 0xE00:
            if ((location & 0x00F0) < 0xA0)
                return oam[location - 0xFE00];
            else
            {
                //Nintendo says 0xFEA0 - 0xFF00 is unusable.
                std::cout << "MMU: WARNING: Attempted read from unusable memory address " << uctohs(location) << std::endl;
                return badmem[location - 0xFEA0];
            }
            case 0xF00:
            //These contain IO controllers.
            if (location < 0xFF80)
                return ioram[location - 0xFF00];
            else
                return zram[location - 0xFF80];
        }
    }
    return 0x00;
}

unsigned short MMU::r16(unsigned short location)
{
    return r8(location) + (r8(location+1) << 8);
}

void MMU::w8(unsigned char value, unsigned short location)
{
    switch (location & 0xF000)
    {
        case 0x0000:
        case 0x1000:
        //NONE: write to rombank
        //MBC1, MBC5: SRAM enable
        //MBC2: continue
        if (mmukind == NONE)
        {
            rombanks[location] = value;
            break;
        }
        if (mmukind != MBC2)
        {
            sramenabled = (value == 0x0A);
            break;
        }
        case 0x2000:
        //NONE, MBC1, MBC2: continue
        //MBC5: 8th LSD of ROM bank number
        if (mmukind == MBC5)
        {
            mbc5bits18 = value;
            //write selected rombank into rombank 1
            //if it exists
            int bankct = romsize / (16 * 1024);
            if (mbc5bits18 < bankct)
                for (int i = 0; i < 0x4000; i++)
                    rombanks[i + 0x4000] = rom[mbc5bits18 * 0x4000 + i];
            //complete regardless
            break;
        }
        case 0x3000:
        //NONE: continue
        //MBC1: change ROM bank, change RAM bank
        //MBC2: enable RAM / change ROM bank
        //MBC5: 9th bit of ROM bank number
        if (mmukind == MBC1)
        {
            int bankct = romsize / (16 * 1024);
            unsigned char chosenbank;
            //if there are more than 32 banks, look at 7 bits of the
            //written value
            if (bankct > 32)
                chosenbank = value & 0x7F;
            //otherwise, look at 5 bits
            else
                chosenbank = value & 0x1F;
            //get value of 2-bit register
            char twobitr = (value & 0x60) >> 5;
            //if selected bank is higher than the number of
            //banks available, mask it to the number of bits needed
            if (chosenbank >= bankct)
            {
                int bitsneeded = std::ceil(std::log2(bankct));
                unsigned char mask = 0;
                //set mask
                for (int i = 0; i < bitsneeded; i++)
                    mask |= (1 << i);
                //mask chosenbank
                chosenbank &= mask;
            }
            //00->01 transition
            if ((value & 0x1F) == 0 && bankct > 16)
                chosenbank++;
            //if in mode 0
            if (!mbc1mode)
            {
                //write selected bank into rombank 1
                for (int i = 0; i < 0x4000; i++)
                    rombanks[i + 0x4000] = rom[0x4000 * chosenbank + i];
            }
            //if in mode 1
            else
            {
                //write selected bank into rombank 1
                for (int i = 0; i < 0x4000; i++)
                    rombanks[i + 0x4000] = rom[0x4000 * chosenbank + i];
                //depending on twobitr, write bank 0x00, 0x20, 0x40 or 0x60
                //into bank 0 if it exists
                chosenbank = 0x10 * twobitr * 2;
                //if this value is within the number of banks available,
                //write the requested bank to bank 0
                if (chosenbank < bankct)
                    for (int i = 0; i < 0x4000; i++)
                        rombanks[i] = rom[0x4000 * chosenbank + i];
                //if there is more than one sram bank and sram is enabled
                if (sramnumbanks > 1 && sramenabled)
                {
                    //write the contents of the current sram bank into sram
                    for (int i = 0; i < 0x2000; i++)
                        sram[srambanknum * 0x2000 + i] = srambank[i];
                    //update srambanknum
                    srambanknum = twobitr;
                    //write the contents of the selected sram bank into
                    //the sram bank
                    for (int i = 0; i < 0x2000; i++)
                        srambank[i] = sram[srambanknum * 0x2000 + i];
                }
            }
            break;
        }
        if (mmukind == MBC2)
        {
            char bit8 = (location >> 8) & 1;
            //if bit 8 is clear, enable / disable RAM
            if (!bit8)
            {
                sramenabled = (value == 0x0A);
                break;
            }
            //if bit 8 is set, select ROM bank
            //get selected ROM bank number (this is the bottom 4 bits of the value)
            value &= 0x0F;
            int bankct = romsize / (16 * 1024);
            //if the selected ROM bank exists
            if (value < bankct)
                //write selected bank into rombank 1
                for (int i = 0; i < 0x4000; i++)
                    rombanks[i + 0x4000] = rom[0x4000 * value + i];
            break;
        }
        if (mmukind == MBC5)
        {
            //store 9th bit
            mbc5bit9 = value & 1;
            //get total requested ROM bank
            short totalrb = (mbc5bit9 << 8) + mbc5bits18;
            //write selected rombank into rombank 1
            //if it exists
            int bankct = romsize / (16 * 1024);
            if (totalrb < bankct)
                for (int i = 0; i < 0x4000; i++)
                    rombanks[i + 0x4000] = rom[totalrb * 0x4000 + i];
            //complete regardless
            break;
        }
        case 0x4000:
        case 0x5000:
        //NONE: continue
        //MBC1: select SRAM bank / ROM bank in ROM bank 0 if mode 1
        //MBC2: continue
        //MBC5: select SRAM bank
        if (mmukind == MBC1)
        {
            //if in mode 0, this does nothing
            //since writing here only affects
            //which SRAM bank is loaded and which
            //rombank is in rombank 0
            //so check if in mode 1
            if (mbc1mode)
            {
                //get the value in the two-bit register
                char twobitr = value & 0x3;
                //depending on twobitr, write bank 0x00, 0x20, 0x40 or 0x60
                //into bank 0 if it exists
                unsigned char chosenbank = 0x10 * twobitr * 2;
                int bankct = romsize / (16 * 1024);
                //if this value is within the number of banks available,
                //write the requested bank to bank 0
                if (chosenbank < bankct)
                    for (int i = 0; i < 0x4000; i++)
                        rombanks[i] = rom[0x4000 * chosenbank + i];
                //if there is more than one sram bank and sram is enabled
                if (sramnumbanks > 1 && sramenabled)
                {
                    //write the contents of the current sram bank into sram
                    for (int i = 0; i < 0x2000; i++)
                        sram[srambanknum * 0x2000 + i] = srambank[i];
                    //update srambanknum
                    srambanknum = twobitr;
                    //write the contents of the selected sram bank into
                    //the sram bank
                    for (int i = 0; i < 0x2000; i++)
                        srambank[i] = sram[srambanknum * 0x2000 + i];
                }
            }   
            break;
        }
        if (mmukind == MBC5)
        {
            unsigned char selectedbank = value;
            //if a value between 0x0 and 0xF was written
            //and if sram has a bank #selectedbank and sram is enabled
            if (selectedbank < 0x10 && sramnumbanks > selectedbank && sramenabled)
            {
                //write the contents of the current sram bank into sram
                for (int i = 0; i < 0x2000; i++)
                    sram[srambanknum * 0x2000 + i] = srambank[i];
                //update srambanknum
                srambanknum = selectedbank;
                //write the contents of the selected sram bank into
                //the sram bank
                for (int i = 0; i < 0x2000; i++)
                    srambank[i] = sram[srambanknum * 0x2000 + i];
            }
            break;
        }
        case 0x6000:
        case 0x7000:
        //NONE: Write memory
        //MBC1: Change mode
        //MBC2, MBC5: None
        if (mmukind == NONE)
            rombanks[location] = value;
        if (mmukind == MBC1)
        {
            //look at the last bit of the written value
            //set mode to mode 0 or 1 depending on last bit
            mbc1mode = value & 0x01;
            break;
        }
        break;
        case 0x8000:
        case 0x9000:
        //write the value to vram
        vram[location - 0x8000] = value;
        break;
        case 0xA000:
        case 0xB000:
        //NONE, MBC1, MBC5: Write memory if SRAM attached and enabled
        //MBC2: Write 4 lower bits of memory if SRAM attached and enabled
        if (sramnumbanks)
        {
            if (mmukind != MBC2)
            {
                //write to the current sram bank if it's enabled
                if (sramenabled)
                    srambank[location - 0xA000] = value;
                else
                    std::cout << "MMU: WARNING: Attempt to write to disabled SRAM" << std::endl;
                break;
            }
            else
            {
                if (sramenabled)
                {
                    location -= 0xA000;
                    //Write lower 4 bits to all 16 echos of 512 B of RAM
                    for (int i = 0; i < 0xC000; i += 0x200)
                        srambank[location + i] = value & 0xF;
                }
                else
                    std::cout << "MMU: WARNING: Attempt to write to disabled SRAM" << std::endl;
                break;
            }
        }
        break;
        case 0xC000:
        case 0xD000:
        //write to wram
        wram[location - 0xC000] = value;
        break;
        //0xE000 - 0xFDFF contains an unusable echo
        //of wram
        case 0xE000:
        std::cout << "MMU: WARNING: Attempted write to unusable memory address " << uctohs(location) << std::endl;
        break;
        case 0xF000:
        switch (location & 0x0F00)
        {
            case 0x000:
            case 0x100:
            case 0x200:
            case 0x300:
            case 0x400:
            case 0x500:
            case 0x600:
            case 0x700:
            case 0x800:
            case 0x900:
            case 0xA00:
            case 0xB00:
            case 0xC00:
            case 0xD00:
            std::cout << "MMU: WARNING: Attempted write to unusable memory address " << uctohs(location) << std::endl;
            break;
            case 0xE00:
            //write to graphics ram
            if ((location & 0x00F0) < 0xA0)
                oam[location - 0xFE00] = value;
            else
            {
                //Nintendo says 0xFEA0 - 0xFF00 is unusable.
                std::cout << "MMU: WARNING: Attempted write to unusable memory address " << uctohs(location) << std::endl;
                badmem[location - 0xFEA0] = value;
            }
            break;
            case 0xF00:
            //These contain IO controllers.
            if (location < 0xFF80)
            {
                //DIV reset
                if (location == 0xFF04)
                    ioram[0x04] = 0;
                //DMA transfer
                else if (location == 0xFF46)
                {
                    unsigned char upper8 = value;
                    if (upper8 > 0xDF)
                    {
                        std::cout << "MMU: Warning: Cannot start DMA transfer from 0x" << uctohs(upper8)
                            << "; ignored" << std::endl;
                        break;
                    }
                    for (int i = 0; i < 0xA0; i++)
                        oam[i] = r8((upper8 << 8) + i);
                }
                else
                    ioram[location - 0xFF00] = value;
            }
            //otherwise write to zram
            else
            {
                zram[location - 0xFF80] = value;
            }
        }
        break;
    }
}

void MMU::w16(unsigned short value, unsigned short location)
{
    //write lower 8 bits to the address
    w8(location, value & 0xFF);
    //write upper 8 bits to the next address
    w8(location+1, (value >> 8));
}