#include "CPU.h"
#include <iostream>

CPU::CPU(std::string & rompath)
{
    instruction = "";
    ime = false;
    halt = false;
    this->mmu = MMU(rompath);
    d_message = new char[1024];
    msgsize = 0;
    die = false;
    reset();
}
//read registers from reserve registers
void CPU::rrfrs()
{
    a = ra;
    b = rb;
    c = rc;
    d = rd;
    e = re;
    f = rf;
    h = rh;
    l = rl;
}
//write registers to reserve registers
void CPU::wrtrs()
{
    ra = a;
    rb = b;
    rc = c;
    rd = d;
    re = e;
    rf = f;
    rh = h;
    rl = l;
}
//reset all register values
void CPU::reset()
{
    a = 0;
    b = 0;
    c = 0;
    d = 0;
    e = 0;
    f = 0;
    h = 0;
    l = 0;
    pc = 0;
    sp = 0;
    //reset reserve values
    wrtrs();
}

//make a string 20 chars long
std::string CPU::offset(std::string badstring)
{
    std::string out = badstring;
    for (int i = 0; i < 20 - badstring.length(); i++)
        out += " ";
    return out;
}

//convert unsigned char to string
std::string CPU::uchar_to_hex(unsigned char value)
{
    std::string out = "";
    int values[] = { value >> 4, (value & 0x0F) };
    for (int i = 0; i < 2; i++)
    {
        if (values[i] < 0x0A)
            out += std::string(1, 48 + values[i]);
        else
            out += std::string(1, 55 + values[i]);
    }
    return out;
}

//convert unsigned short to string
std::string CPU::ushort_to_hex(unsigned short value)
{
    return uchar_to_hex(value >> 8) + uchar_to_hex(value & 0xFF);
}

std::string CPU::dregp_to_str(unsigned char* ureg)
{
    std::string dr;
    if (ureg == &a)
        dr = "af";
    else if (ureg == &b)
        dr = "bc";
    else if (ureg == &d)
        dr = "de";
    else if (ureg == &h)
        dr = "hl";
    else
        dr = "!";
    return dr;
}

std::string CPU::regp_to_str(unsigned char* reg8)
{
    std::string r;
    if (reg8 == &a)
        r = "a";
    else if (reg8 == &b)
        r = "b";
    else if (reg8 == &c)
        r = "c";
    else if (reg8 == &d)
        r = "d";
    else if (reg8 == &e)
        r = "e";
    else if (reg8 == &f)
        r = "f";
    else if (reg8 == &h)
        r = "h";
    else if (reg8 == &l)
        r = "l";
    else
    {
        r = "!";
    }
    return r;
}

char CPU::abs(char in)
{
    if (in < 0);
        return -in;
    return in;
}

std::string CPU::flag_to_str(Flag flag)
{
    switch (flag)
    {
        case ZERO:
            return "z";
        case SUB:
            return "n";
        case HALF:
            return "h";
        case CARRY:
            return "c";
        default:
            return "!";
    }
}

void CPU::serial_update()
{
    if (mmu.r8(0xFF02) == 0x81)
    {
        char c = mmu.r8(0xFF01);
        d_message[msgsize++] = c;
        mmu.w8(0, 0xFF02);
    }
}

void CPU::serial_print()
{
    if (d_message[0])
    {
        std::cout << "SERIAL OUT: " << std::string(d_message, d_message + 1024) << std::endl;
    }
}

void CPU::request_interrupt(IntType interrupt)
{
    switch (interrupt)
    {
        case VB:
            mmu.w8(mmu.r8(0xFFFF) | 1, 0xFFFF);
            break;
        case LCDSTAT:
            mmu.w8(mmu.r8(0xFFFF) | (1 << 1), 0xFFFF);
            break;
        case TIMER:
            mmu.w8(mmu.r8(0xFFFF) | (1 << 2), 0xFFFF);
            break;
        case SERIAL:
            mmu.w8(mmu.r8(0xFFFF) | (1 << 3), 0xFFFF);
            break;
        case JOYPAD:
            mmu.w8(mmu.r8(0xFFFF) | (1 << 4), 0xFFFF);
            break;
    }
}

void CPU::step()
{
    if (!halt)
    {
        unsigned short curpc = pc;
        do_op();
        std::string cur_pc = ushort_to_hex(curpc);
        if (instruction != "")
        {
            std::cout << "0x" << cur_pc << ": " << instruction
                << " af = 0x" << uchar_to_hex(a) << uchar_to_hex(f)
                << (checkflag(ZERO) ? " z" : " -")
                << (checkflag(SUB) ? "n" : "-")
                << (checkflag(HALF) ? "h" : "-")
                << (checkflag(CARRY) ? "c" : "-")
                << " "
                << " bc = 0x" << uchar_to_hex(b) << uchar_to_hex(c)
                << " de = 0x" << uchar_to_hex(d) << uchar_to_hex(e)
                << " hl = 0x" << uchar_to_hex(h) << uchar_to_hex(l)
                << " sp = 0x" << ushort_to_hex(sp)
                << " pc = 0x" << ushort_to_hex(pc)
                << std::endl;
            //serial_update();
            //serial_print();
        }
    }

    else
    {
        if (mmu.r8(0xFF0F))
            halt = false;
    }
    if (mmu.r8(0xFFFF) == 0)
        ime = false;
    //handle interrupts
    if (ime)
    {
        //all enabled interrupts
        unsigned char ie = mmu.r8(0xFFFF);
        //all interrupt flags
        unsigned char iflags = mmu.r8(0xFF0F);
        //interrupt handler locations
        //VBLANK LCD_STATUS TIMER_OVERFLOW SERIAL_LINK JOYPAD_PRESS
        unsigned short handlers[] = { 0x0040, 0x0048, 0x0050, 0x0058, 0x0060 };
        for (int i = 0; i < 5; i++)
        {
            unsigned char enabled = (ie & (1 << i)) >> i;
            unsigned char set = (iflags & (1 << i)) >> i;
            //if there is an interrupt that is both enabled
            //and set
            if (enabled && set)
            {
                //clear interrupt flag
                set &= ~(1 << i);
                //write current pc to sp
                mmu.w8(pc >> 8, sp - 1);
                mmu.w8(pc & 0xFF, sp - 2);
                //jump to handler
                pc = handlers[i];
                //prevent further interrupts
                ime = false;
                //only handle one interrupt at a time
                break;
            }
        }
    }
}