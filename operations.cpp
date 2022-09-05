#include "CPU.h"
#include <iostream>

void CPU::setflag(Flag flag)
{
    switch (flag)
    {
        case ZERO:
            f |= 0x80;
            break;
        case SUB:
            f |= 0x40;
            break;
        case HALF:
            f |= 0x20;
            break;
        case CARRY:
            f |= 0x10;
            break;
        default:
            return;
    }
}

void CPU::clearflag(Flag flag)
{
    switch (flag)
    {
        case ZERO:
            f &= ~0x80;
            break;
        case SUB:
            f &= ~0x40;
            break;
        case HALF:
            f &= ~0x20;
            break;
        case CARRY:
            f &= ~0x10;
            break;
        default:
            return;
    }
}

void CPU::condflag(bool condition, Flag flag)
{
    if (condition)
        setflag(flag);
    else
        clearflag(flag);
}

bool CPU::checkflag(Flag flag)
{
    switch (flag)
    {
        case ZERO:
            return (f & 0x80) != 0;
        case SUB:
            return (f & 0x40) != 0;
        case HALF:
            return (f & 0x20) != 0;
        case CARRY:
            return (f & 0x10) != 0;
        default:
            return 0;
    }
}

//No operation
void CPU::NOP()
{
    instruction = "nop";
    instruction = offset(instruction);
    m = 1;
    pc++;
}

//Write an imm16 to a register pair
void CPU::LD_DR_IMM16(unsigned char* ureg, unsigned char* lreg)
{
    pc++;
    unsigned short value = mmu.r16(pc);
    instruction = "ld " + dregp_to_str(ureg) + ", 0x" + ushort_to_hex(value);
    instruction = offset(instruction);
    *ureg = value >> 8;
    *lreg = value & 0xFF;
    pc += 2;
    m = 3;
}

void CPU::LD_SP_IMM16()
{
    pc++;
    sp = mmu.r16(pc);
    instruction = offset("ld sp, 0x" + ushort_to_hex(sp));
    pc += 2;
    m = 3;
}

//Write a register's value to the memory stored in a register pair
void CPU::STR_DR_R(unsigned char* ureg, unsigned char* lreg, unsigned char* reg8)
{
    instruction = offset("ld (" + dregp_to_str(ureg) + "), " + regp_to_str(reg8));
    mmu.w8(*reg8, (*ureg << 8) + *lreg);
    pc++;
    m = 2;
}

//Write A to the memory stored in HL + 1
void CPU::STR_MEMPLUS_R()
{
    instruction = offset("ld (hl+), a");
    mmu.w8(a, (h << 8) + l);
    if (l == 0xFF)
        h++;
    l++;
    pc++;
    m = 2;
}

//Write A to the memory stored in HL - 1
void CPU::STR_MEMMINUS_R()
{
    instruction = offset("ld (hl-), a");
    mmu.w8(a, (h << 8) + l);
    if (l == 0x00)
        h--;
    l--;
    pc++;
    m = 2;
}

//Increment a register pair by 1
void CPU::INC_DR(unsigned char* ureg, unsigned char* lreg)
{
    instruction = offset("inc " + dregp_to_str(ureg));
    if (*lreg == 0xFF)
        (*ureg)++;
    (*lreg)++;
    pc++;
    m = 2;
}

//Increment SP by 1
void CPU::INC_SP()
{
    instruction = offset("inc sp");
    sp++;
    pc++;
    m = 2;
}

//Increment a register by 1
void CPU::INC_R(unsigned char* reg8)
{
    instruction = offset("inc " + regp_to_str(reg8));
    //zero flag
    condflag(*reg8 == 0xFF, ZERO);
    //subtract flag
    clearflag(SUB);
    //half-carry flag
    condflag((*reg8 & 0xF) == 0xF, HALF);
    (*reg8)++;
    pc++;
    m = 1;
}

//Increment the memory address at HL by 1
void CPU::INC_MEM()
{
    instruction = offset("inc hl");
    //read memory
    unsigned char value = mmu.r8((h << 8) + l);
    //zero flag
    condflag(value == 0xFF, ZERO);
    //subtract flag
    clearflag(SUB);
    //half-carry flag
    condflag((value & 0xF) == 0xF, HALF);
    value++;
    //write memory
    mmu.w8(value, (h << 8) + l);
    pc++;
    m = 3;
}

//Decrement a register by 1
void CPU::DEC_R(unsigned char* reg8)
{
    instruction = offset("dec " + regp_to_str(reg8));
    //zero flag
    condflag(*reg8 == 0x01, ZERO);
    //subtract flag
    setflag(SUB);
    //half-carry flag
    condflag(((*reg8 & 0xF) - 1) < 0, HALF);
    (*reg8)--;
    pc++;
    m = 1;
}

//Increment the memory address at HL by 1
void CPU::DEC_MEM()
{
    instruction = offset("dec (hl)");
    //read memory
    unsigned char value = mmu.r8((h << 8) + l);
    //zero flag
    condflag(value == 0x01, ZERO);
    //subtract flag
    setflag(SUB);
    //half-carry flag
    condflag(((value & 0xF) - 1) < 0, HALF);
    value--;
    //write memory
    mmu.w8(value, (h << 8) + l);
    pc++;
    m = 3;
}

//Load an imm8 into register
void CPU::LD_R_IMM8(unsigned char* reg8)
{
    pc++;
    *reg8 = mmu.r8(pc);
    instruction = offset("ld " + regp_to_str(reg8) + ", 0x" + uchar_to_hex(*reg8));
    pc++;
    m = 2;
}

//Load an imm8 into the memory in HL
void CPU::LD_MEM_IMM8()
{
    pc++;
    instruction = offset("ld (hl), 0x" + uchar_to_hex(mmu.r8(pc)));
    mmu.w8(mmu.r8(pc), (h << 8) + l);
    pc++;
    m = 3;
}

//Rotate A left with carry
void CPU::RLC()
{
    instruction = offset("rlca");
    //Clear zero, subtraction, and half-carry flags
    clearflag(ZERO);
    clearflag(SUB);
    clearflag(HALF);
    short temp = (a << 1) + (a >> 7);
    //carry flag
    condflag(temp > 0xFF, CARRY);
    //set a
    a = temp & 0xFF;
    pc++;
    m = 1;
}

//Rotate A left
void CPU::RL()
{
    instruction = offset("rla");
    //clear zero, sub, half-carry flags
    clearflag(ZERO);
    clearflag(SUB);
    clearflag(HALF);
    short temp = (a << 1) + checkflag(CARRY);
    //carry flag
    condflag(temp > 0xFF, CARRY);
    a = temp & 0xFF;
    pc++;
    m = 1;
}

//Write SP to a memory IMM16
void CPU::STR_IMM16_SP()
{
    pc++;
    unsigned short value = mmu.r16(pc);
    instruction = offset("ld (0x" + ushort_to_hex(value) + "), sp");
    mmu.w16(sp, value);
    pc += 2;
    m = 5;
}

//Add a double register to HL
void CPU::ADD_DR_DR(unsigned char* ureg, unsigned char* lreg)
{
    instruction = offset("add hl, " + dregp_to_str(ureg));
    //subtraction flag
    clearflag(SUB);
    int total = (h << 8) + l + ((*ureg) << 8) + *lreg;
    //half-carry flag
    condflag(int(((h << 8) + l) & 0xFFF) + int((((*ureg) << 8) + *lreg) & 0xFFF) > 0xFFF, HALF);
    //carry flag
    condflag(total > 0xFFFF, CARRY);
    //store in h and l
    h = (total & 0xFFFF) >> 8;
    l = (total & 0xFFFF) & 0xFF;
    pc++;
    m = 2;
}

//Add SP to HL
void CPU::ADD_DR_SP()
{
    instruction = offset("add hl, sp");
    //subtraction flag
    clearflag(SUB);
    int total = (h << 8) + l + sp;
    //half-carry flag
    condflag(int(((h << 8) + l) & 0xFFF) + int(sp & 0xFFF) > 0xFFF, HALF);
    //store in h and l
    h = (total & 0xFFFF) >> 8;
    l = (total & 0xFFFF) & 0xFF;
    pc++;
    m = 2;
}

//Load a value from a register pair and store it in A
void CPU::LD_R_DR(unsigned char* ureg, unsigned char* lreg)
{
    instruction = offset("ld a, (" + dregp_to_str(ureg) + ")");
    unsigned short index = ((*ureg) << 8) + *lreg;
    a = mmu.r8(index);
    pc++;
    m = 2;
}

//Load a value from HL + 1 and store it in A
void CPU::LD_R_MEMPLUS()
{
    instruction = offset("ld (hl+), a");
    a = mmu.r8((h << 8) + l);
    if (l == 0xFF)
        h++;
    l++;
    pc++;
    m = 2;
}

//Load a value from HL - 1 and store it in A
void CPU::LD_R_MEMMINUS()
{
    instruction = offset("ld (hl-), a");
    a = mmu.r8((h << 8) + l);
    if (l == 0x00)
        h--;
    l--;
    pc++;
    m = 2;
}

//Decrement a double register
void CPU::DEC_DR(unsigned char* ureg, unsigned char* lreg)
{
    instruction = offset("dec " + dregp_to_str(ureg));
    unsigned short temp = (*ureg << 8) + *lreg;
    temp--;
    *ureg = temp >> 8;
    *lreg = temp & 0xFF;
    pc++;
    m = 2;
}

//Decrement SP
void CPU::DEC_SP()
{
    instruction = offset("dec sp");
    sp--;
    pc++;
    m = 2;
}

//Rotate A right with carry
void CPU::RRC()
{
    instruction = offset("rrca");
    //zero flag
    clearflag(ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    short t = (a >> 1) + ((a & 1) << 7) + ((a & 1) << 8);
    //carry flag
    condflag(t > 0xFF, CARRY);
    a = t & 0xFF;
    pc++;
    m = 1;
}

//Rotate A right without carry
void CPU::RR()
{
    instruction = offset("rra");
    //zero flag
    clearflag(ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    short t = (a >> 1) + (checkflag(CARRY) << 7) + ((a & 1) << 8);
    //carry flag
    condflag(t > 0xFF, CARRY);
    a = t & 0xFF;
    pc++;
    m = 1;
}

//do nothing in GB mode
void CPU::STOP()
{
    instruction = offset("stop");
    //clear DIV
    mmu.w8(0, 0xFF04);
    pc += 2;
    m = 1;
}

//add a signed 8-bit value to pc
void CPU::JUMP_IMMS8()
{
    char value = char((mmu.r8(pc+1) ^ 0x80) - 0x80);
    instruction = "jr ";
    if (abs(value) > value)
        instruction += ("-0x" + uchar_to_hex(abs(value)));
    else
        instruction += ("0x" + uchar_to_hex(value));
    instruction = offset(instruction);
    pc += value;
    pc += 2;
    m = 3;
}

//if the given flag, then jump by a signed 8-bit value
void CPU::JUMP_FLAG_IMMS8(Flag flag)
{
    char value = char((mmu.r8(pc+1) ^ 0x80) - 0x80);
    instruction = "jr " + flag_to_str(flag) + ", ";
    if (abs(value) > value)
        instruction += ("-0x" + uchar_to_hex(abs(value)));
    else
        instruction += ("0x" + uchar_to_hex(value));
    instruction = offset(instruction);
    if (checkflag(flag))
    {
        pc += value;
        pc += 2;
        m = 3;
    }
    else
    {
        pc += 2;
        m = 2;
    }
}

//if not the given flag, then jump by a signed 8-bit value
void CPU::JUMP_NFLAG_IMMS8(Flag flag)
{
    char value = char((mmu.r8(pc+1) ^ 0x80) - 0x80);
    instruction = "jr n" + flag_to_str(flag) + ", ";
    if (abs(value) > value)
        instruction += ("-0x" + uchar_to_hex(abs(value)));
    else
        instruction += ("0x" + uchar_to_hex(value));
    instruction = offset(instruction);
    pc += 2;
    if (!checkflag(flag))
    {
        pc += value;
        m = 3;
    }
    else
        m = 2;
}

//decimal adjust a
void CPU::DAA()
{
    instruction = offset("daa");
    unsigned char corr = 0;
    corr |= checkflag(HALF) ? 0x06 : 0x00;
    //half-carry flag
    clearflag(HALF);
    corr |= checkflag(CARRY) ? 0x60 : 0x00;
    if (checkflag(SUB))
        a -= corr;
    else
    {
        corr |= (a & 0xF) > 0x09 ? 0x06 : 0x00;
        corr |= a > 0x99 ? 0x60 : 0x00;
        a += corr;
    }
    //zero flag
    condflag((a & 0xFF) == 0, ZERO);
    //carry flag
    condflag(corr & 0x60, CARRY);
    pc++;
    m = 1;
}

//CPL (dunno why it's called that...)
void CPU::CPL()
{
    instruction = offset("cpl");
    a ^= 0xFF;
    //subtraction flag
    setflag(SUB);
    //half-carry flag
    setflag(HALF);
    pc++;
    m = 1;
}

//SCF (dunno why it's called that...)
void CPU::SCF()
{
    instruction = offset("scf");
    unsigned char temp = 0b00010000;
    f &= 0b10000000;
    f |= temp;
    pc++;
    m = 1;
}

//CCF (dunno why it's called that...)
void CPU::CCF()
{
    instruction = offset("ccf");
    unsigned char temp = (f & 0b00010000) ^ 0b00010000;
    f &= 0b10000000;
    f |= temp;
    pc++;
    m = 1;
}

//load a value from one register into another
void CPU::LD_R_R(unsigned char* reg1, unsigned char* reg2)
{
    instruction = offset("ld " + regp_to_str(reg1) + ", " + regp_to_str(reg2));
    *reg1 = *reg2;
    pc++;
    m = 1;
}

//load a value from memory pointed at by HL into a register
void CPU::LD_R_MEM(unsigned char* reg8)
{
    instruction = offset("ld " + regp_to_str(reg8) + ", (hl)");
    *reg8 = mmu.r8((h << 8) + l);
    pc++;
    m = 2;
}

//Write a register to a value in memory
void CPU::STR_MEM_R(unsigned char* reg8)
{
    instruction = offset("ld (hl), " + regp_to_str(reg8));
    mmu.w8(*reg8, (h << 8) + l);
    pc++;
    m = 2;
}

//halt operation
void CPU::HALT()
{
    instruction = offset("halt");
    halt = true;
    m = 1;
}

//add two registers and store the result
//in the first
void CPU::ADD_A_R(unsigned char* reg1)
{
    instruction = offset("add a, " + regp_to_str(reg1));
    //zero flag
    unsigned char out = a + *reg1;
    condflag(out == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    condflag((a & 0xF) + (*reg1 & 0xF) > 0xF, HALF);
    //carry flag
    condflag(short(a + *reg1) > 0xFF, CARRY);
    //set reg1
    a = out;
    pc++;
    m = 1;
}

//Add A to the memory address in HL, and store the value
//in A
void CPU::ADD_A_MEM()
{
    unsigned char temp = mmu.r8((h << 8) + l);
    ADD_A_R(&temp);
    instruction = offset("add a, (hl)");
    m = 2;
}

//Store the value of A minus a register in A.
void CPU::SUB_A_R(unsigned char* reg1)
{
    instruction = offset("sub " + regp_to_str(reg1));
    short out = a - *reg1;
    //zero flag
    condflag(out == 0, ZERO);
    //subtraction flag
    setflag(SUB);
    //half-carry flag
    condflag(char((a & 0xF) - (*reg1 & 0xF)) < 0, HALF);
    //carry flag
    condflag(out < 0, CARRY);
    //store subtraction
    a -= *reg1;
    pc++;
    m = 1;
}

//Store the value of A minus the memory address in HL in A.
void CPU::SUB_A_MEM()
{
    //get memory
    unsigned char temp = mmu.r8((h << 8) + l);
    //perform subtraction
    SUB_A_R(&temp);
    instruction = offset("sub (hl)");
    m = 2;
}

//Add another register to A with carry, and store the result in A.
void CPU::ADC_A_R(unsigned char* reg1)
{
    instruction = offset("adc a, " + regp_to_str(reg1));
    unsigned char out = a + *reg1 + checkflag(CARRY);
    //zero flag
    condflag(out == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    condflag(char((a & 0xF) + (*reg1 & 0xF) + checkflag(CARRY)) > 0xF, HALF);
    //carry flag
    condflag(short(a + *reg1 + checkflag(CARRY)) > 0xFF, CARRY);
    //perform operation
    a = out;
    pc++;
    m = 1;
}

//Add the value pointed at by HL to A with carry, and store the result in A.
void CPU::ADC_A_MEM()
{
    unsigned char temp = mmu.r8((h << 8) + l);
    ADC_A_R(&temp);
    instruction = offset("adc a, (hl)");
    m = 2;
}

//Subtract a register and carry from A, and store the result in A.
void CPU::SBC_A_R(unsigned char* reg1)
{
    instruction = offset("sbc a, " + regp_to_str(reg1));
    //zero flag
    condflag(a - *reg1 - checkflag(CARRY) == 0, ZERO);
    //subtraction flag
    setflag(SUB);
    //half-carry flag
    condflag(char((a & 0xF) - (*reg1 & 0xF) - checkflag(CARRY)) < 0, HALF);
    //carry flag
    condflag(short(a - *reg1 - checkflag(CARRY)) < 0, CARRY);
    //perform operation
    a -= (*reg1 + checkflag(CARRY));
    pc++;
    m = 1;
}

//Subtract the memory value pointed at by HL and carry from A, and store the result in A.
void CPU::SBC_A_MEM()
{
    unsigned char temp = mmu.r8((h << 8) + l);
    SBC_A_R(&temp);
    instruction = offset("sbc a, (hl)");
    m = 2;
}

//Store the bitwise AND of a register and A in A.
void CPU::AND_R(unsigned char* reg1)
{
    instruction = offset("and " + regp_to_str(reg1));
    a &= *reg1;
    //zero flag
    condflag(a == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    setflag(HALF);
    //carry flag
    clearflag(CARRY);
    pc++;
    m = 1;
}

//Store the bitwise AND of the memory pointed to by HL and A in A.
void CPU::AND_MEM()
{
    unsigned char temp = mmu.r8((h << 8) + l);
    AND_R(&temp);
    instruction = offset("and (hl)");
    m = 2;
}

//Store the bitwise OR of a register and A in A.
void CPU::OR_R(unsigned char* reg1)
{
    instruction = offset("or " + regp_to_str(reg1));
    a |= *reg1;
    //zero flag
    condflag(a == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    setflag(HALF);
    //carry flag
    clearflag(CARRY);
    pc++;
    m = 1;
}

//Store the bitwise OR of the memory pointed to by HL and A in A.
void CPU::OR_MEM()
{
    unsigned char temp = mmu.r8((h << 8) + l);
    OR_R(&temp);
    instruction = offset("or (hl)");
    m = 2;
}

//Store the bitwise XOR of a register and A in A.
void CPU::XOR_R(unsigned char* reg1)
{
    instruction = offset("xor " + regp_to_str(reg1));
    a ^= *reg1;
    //zero flag
    condflag(a == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    clearflag(CARRY);
    pc++;
    m = 1;
}

//Store the bitwise AND of the memory pointed to by HL and A in A.
void CPU::XOR_MEM()
{
    unsigned char temp = mmu.r8((h << 8) + l);
    XOR_R(&temp);
    instruction = offset("xor (hl)");
    m = 2;
}

//Compare a - register
void CPU::CP_R(unsigned char* reg1)
{
    ra = a;
    SUB_A_R(reg1);
    a = ra;
    instruction = offset("cp " + regp_to_str(reg1));
}

//Compare a - (memory pointed at by HL)
void CPU::CP_MEM()
{
    ra = a;
    SUB_A_MEM();
    a = ra;
    instruction = offset("cp (hl)");
}

//Return if not flag
void CPU::RET_NFLAG(Flag flag)
{
    instruction = offset("ret n" + flag_to_str(flag));
    if (!flag)
    {
        pc = mmu.r8(sp + 1) << 8;
        pc |= mmu.r8(sp);
        sp += 2;
        m = 5;
    }
    else
    {
        pc++;
        m = 2;
    }
}

//Return if flag
void CPU::RET_FLAG(Flag flag)
{
    instruction = offset("ret " + flag_to_str(flag));
    if (flag)
    {
        pc = mmu.r8(sp + 1) << 8;
        pc |= mmu.r8(sp);
        sp += 2;
        m = 5;
    }
    else
    {
        pc++;
        m = 2;
    }
}

//Pop double register
void CPU::POP_DR(unsigned char* ureg, unsigned char* lreg)
{
    instruction = offset("pop " + dregp_to_str(ureg));
    *ureg = mmu.r8(sp + 1);
    *lreg = mmu.r8(sp);
    sp += 2;
    pc++;
    m = 3;
}

//Pop AF
void CPU::POP_AF()
{
    instruction = offset("pop af");
    a = mmu.r8(sp + 1);
    f = mmu.r8(sp) & 0xF0 & 0xF0;
    sp += 2;
    pc++;
    m = 3;
}

//Push double register
void CPU::PUSH_DR(unsigned char* ureg, unsigned char* lreg)
{
    instruction = offset("push " + dregp_to_str(ureg));
    mmu.w8(*ureg, sp - 1);
    mmu.w8(*lreg, sp - 2);
    sp -= 2;
    pc++;
    m = 4;
}

//Jump to passed imm16 if not flag
void CPU::JUMP_NFLAG_IMM16(Flag flag)
{
    pc++;
    unsigned short imm16 = mmu.r16(pc);
    instruction = offset("jp n" + flag_to_str(flag) + ", 0x" + ushort_to_hex(imm16));
    if (!checkflag(flag))
    {
        pc = mmu.r16(imm16);
        m = 4;
    }
    else
    {
        pc += 2;
        m = 3;
    }
}

//Jump to passed imm16 if flag
void CPU::JUMP_FLAG_IMM16(Flag flag)
{
    pc++;
    unsigned short imm16 = mmu.r16(pc);
    instruction = offset("jp " + flag_to_str(flag) + ", 0x" + ushort_to_hex(imm16));
    if (!checkflag(flag))
    {
        pc = mmu.r16(imm16);
        m = 4;
    }
    else
    {
        pc += 2;
        m = 3;
    }
}

//Jump to passed imm16
void CPU::JUMP_IMM16()
{
    pc++;
    unsigned short imm16 = mmu.r16(pc);
    instruction = offset("jp 0x" + ushort_to_hex(imm16));
    pc = imm16;
    m = 4;
}

//Call function at imm16 if not flag
void CPU::CALL_NFLAG_IMM16(Flag flag)
{
    pc++;
    if (!checkflag(flag))
    {
        mmu.w8((pc + 2) >> 8, sp - 1);
        mmu.w8((pc + 2) & 0xFF, sp - 2);
        sp -= 2;
        unsigned short value = mmu.r16(pc);
        instruction = offset("call n" + flag_to_str(flag) + ", 0x" + ushort_to_hex(value));
        pc = value;
        m = 6;
    }
    else
    {
        pc += 2;
        m = 3;
    }
}

//Call function at imm16 if flag
void CPU::CALL_FLAG_IMM16(Flag flag)
{
    pc++;
    if (checkflag(flag))
    {
        mmu.w8((pc + 2) >> 8, sp - 1);
        mmu.w8((pc + 2) & 0xFF, sp - 2);
        sp -= 2;
        unsigned short value = mmu.r16(pc);
        instruction = offset("call n" + flag_to_str(flag) + ", 0x" + ushort_to_hex(value));
        pc = value;
        m = 6;
    }
    else
    {
        pc += 2;
        m = 3;
    }
}

//Write PC to SP's memory location and set PC to set value
void CPU::RST_HEX8(unsigned char hex8)
{
    instruction = offset("rst 0x" + uchar_to_hex(hex8));
    pc++;
    mmu.w8(pc >> 8, sp - 1);
    mmu.w8(pc & 0xFF, sp - 2);
    sp -= 2;
    pc = hex8;
    m = 4;
}

//Jump to the value in HL
void CPU::JUMP_HL()
{
    instruction = offset("jp hl");
    pc = (h << 8) + l;
    m = 1;
}

//return nothing; go back to previous function
void CPU::RET()
{
    instruction = offset("ret");
    pc = mmu.r8(sp + 1) << 8;
    pc |= mmu.r8(sp);
    sp += 2;
    m = 4;
}

//set IME and return nothing; go back to previous function
void CPU::RETI()
{
    instruction = offset("reti");
    ime = true;
    pc = mmu.r8(sp + 1) << 8;
    pc |= mmu.r8(sp);
    sp += 2;
    m = 4;
}

//call the function at the provided imm16
void CPU::CALL_IMM16()
{
    pc++;
    mmu.w8((pc + 2) >> 8, sp - 1);
    mmu.w8((pc + 2) & 0xFF, sp - 2);
    sp -= 2;
    unsigned short value = mmu.r16(pc);
    instruction = offset("call 0x" + ushort_to_hex(value));
    pc = value;
    m = 6;
}

//Write A to 0xFF00 + imm8
void CPU::STRO_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    instruction = offset("ldh (0xFF" + uchar_to_hex(temp) + "), a");
    mmu.w8(a, 0xFF00 + temp);
    pc += 2;
    m = 3;
}

//Write the memory at 0xFF00 + imm8 to A
void CPU::LDO_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    instruction = offset("ldh a, (0xFF" + uchar_to_hex(temp) + ")");
    a = mmu.r8(0xFF00 + temp);
    pc += 2;
    m = 3;
}

//Write A to 0xFF00 + C
void CPU::STRO_C()
{
    instruction = offset("ldh (c), a");
    mmu.w8(a, 0xFF00 + c);
    pc++;
    m = 2;
}

//Write the memory at 0xFF00 + C to A
void CPU::LDO_C()
{
    instruction = offset("ldh a, (c)");
    a = mmu.r8(0xFF00 + c);
    pc++;
    m = 2;
}

//Store A in the memory pointed at by an imm16
void CPU::STR_IMM16()
{
    unsigned short addr = mmu.r16(pc + 1);
    instruction = offset("ld (" + ushort_to_hex(addr) + "), a");
    mmu.w8(a, addr);
    pc += 3;
    m = 4;
}

//Load A from the memory pointed at by an imm16
void CPU::LD_IMM16()
{
    unsigned short addr = mmu.r16(pc + 1);
    instruction = offset("ld a, (0x" + ushort_to_hex(addr) + ")");
    a = mmu.r8(addr);
    pc += 3;
    m = 4;
}

//Add an imm8 to SP
void CPU::ADD_SP_IMM8()
{
    //zero flag
    clearflag(ZERO);
    //subtraction flag
    clearflag(SUB);
    char imm8 = char((mmu.r8(pc+1) ^ 0x80) - 0x80);
    instruction = "add sp, ";
    if (abs(imm8) > imm8)
        instruction += ("-0x" + uchar_to_hex(abs(imm8)));
    else
        instruction += ("0x" + uchar_to_hex(imm8));
    instruction = offset(instruction);
    //half-carry flag
    condflag((sp & 0xF) + (imm8 & 0xF) > 0xF, HALF);
    //carry flag
    condflag(short((sp & 0xFF) + imm8) > 0xFF, CARRY);
    sp += imm8;
    pc += 2;
    m = 4;
}

//Write HL to SP
void CPU::LD_SP_HL()
{
    instruction = "ld sp, hl";
    sp = (h << 8) + l;
    pc++;
    m = 2;
}

//Write SP + an imm8 to HL
void CPU::LD_HL_SP_PLUS_IMM8()
{
    char value = mmu.r8(pc + 1);
    instruction = "ld hl, sp + ";
    if (abs(value) > value)
        instruction += ("-0x" + uchar_to_hex(abs(value)));
    else
        instruction += ("0x" + uchar_to_hex(value));
    instruction = offset(instruction);
    unsigned short total = sp + value;
    //zero flag
    clearflag(ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    condflag((sp & 0xF) + (value & 0xF) > 0xF, HALF);
    //carry flag
    condflag((sp & 0xFF) + (value & 0xFF) > 0xFF, HALF);
    h = total >> 8;
    l = total & 0xFF;
    pc += 2;
    m = 3;
}

//Add an imm8 to A
void CPU::ADD_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    ADD_A_R(&temp);
    instruction = offset("add a, 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Subtract an imm8 from A
void CPU::SUB_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    SUB_A_R(&temp);
    instruction = offset("sub 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Bitwise AND of an imm8 and A
void CPU::AND_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    AND_R(&temp);
    instruction = offset("and 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Bitwise OR of an imm8 and A
void CPU::OR_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    OR_R(&temp);
    instruction = offset("or 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Add with carry an imm8 to A
void CPU::ADC_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    ADC_A_R(&temp);
    instruction = offset("adc a, 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Subtract with carry an imm8 to A
void CPU::SBC_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    SBC_A_R(&temp);
    instruction = offset("sbc a, 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Bitwise XOR of an imm8 and A
void CPU::XOR_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    XOR_R(&temp);
    instruction = offset("xor 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Compare an imm8 and A
void CPU::CP_IMM8()
{
    unsigned char temp = mmu.r8(pc + 1);
    CP_R(&temp);
    instruction = offset("cp 0x" + uchar_to_hex(temp));
    pc++;
    m = 2;
}

//Set prefix boolean
void CPU::PREFIX()
{
    instruction = "";
    if (!prefix)
        prefix = true;
    else
    {
        std::cout << "ERROR: CPU: Prefix opcode (0xCB) called more than once consecutively" << std::endl;
        throw 1;
    }
    pc++;
    m = 1;
}

//Disable interrupts
void CPU::DI()
{
    instruction = offset("di");
    ime = false;
    pc++;
    m = 1;
}

//Enable interrupts
void CPU::EI()
{
    instruction = offset("ei");
    ime = true;
    pc++;
    m = 1;
}

//PREFIX'D FUNCTIONS

//Rotate register left with carry
void CPU::RLC_R(unsigned char* reg8)
{
    instruction = offset("rlc " + regp_to_str(reg8));
    short out = (*reg8 << 1) + (*reg8 >> 7);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    *reg8 = out & 0xFF;
    pc++;
    m = 2;
}

//Rotate the memory stored in HL left with carry
void CPU::RLC_MEM()
{
    instruction = offset("rlc (hl)");
    unsigned char value = mmu.r8((h << 8) + l);
    short out = (value << 1) + (value >> 7);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    mmu.w8(out & 0xFF, (h << 8) + l);
    pc++;
    m = 4;
}

//Rotate register left without carry
void CPU::RL_R(unsigned char* reg8)
{
    instruction = offset("rl " + regp_to_str(reg8));
    short out = (*reg8 << 1) + checkflag(CARRY);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    *reg8 = out & 0xFF;
    pc++;
    m = 2;
}

//Rotate memory pointed at by HL left without carry
void CPU::RL_MEM()
{
    instruction = offset("rl (hl)");
    unsigned char value = mmu.r8((h << 8) + l);
    short out = (value << 1) + checkflag(CARRY);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    mmu.w8(out & 0xFF, (h << 8) + l);
    pc++;
    m = 4;
}

//Rotate register left with carry
void CPU::RRC_R(unsigned char* reg8)
{
    instruction = offset("rrc " + regp_to_str(reg8));
    short out = (*reg8 >> 1) + (*reg8 << 7) + ((*reg8 & 1) << 8);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    *reg8 = out & 0xFF;
    pc++;
    m = 2;
}

//Rotate the memory stored in HL left with carry
void CPU::RRC_MEM()
{
    instruction = offset("rrc (hl)");
    unsigned char value = mmu.r8((h << 8) + l);
    short out = (value >> 1) + (value << 7) + ((value & 1) << 8);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    mmu.w8(out & 0xFF, (h << 8) + l);
    pc++;
    m = 4;
}

//Rotate register left without carry
void CPU::RR_R(unsigned char* reg8)
{
    instruction = offset("rr " + regp_to_str(reg8));
    short out = (*reg8 >> 1) + (checkflag(CARRY) << 7) + ((*reg8 & 1) << 8);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    *reg8 = out & 0xFF;
    pc++;
    m = 2;
}

//Rotate memory pointed at by HL left without carry
void CPU::RR_MEM()
{
    instruction = offset("rr (hl)");
    unsigned char value = mmu.r8((h << 8) + l);
    short out = (value >> 1) + (checkflag(CARRY) << 7) + ((value & 1) << 8);
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    mmu.w8(out & 0xFF, (h << 8) + l);
    pc++;
    m = 4;
}

//Shift left (arithmetic) a register
void CPU::SLA_R(unsigned char* reg8)
{
    instruction = offset("sla " + regp_to_str(reg8));
    short out = *reg8 << 1;
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    *reg8 = out & 0xFF;
    pc++;
    m = 2;
}

//Shift left (arithmetic) the memory location pointed at by HL
void CPU::SLA_MEM()
{
    unsigned char value = mmu.r8((h << 8) + l);
    SLA_R(&value);
    instruction = offset("sla (hl)");
    mmu.w8(value, (h << 8) + l);
    m = 4;
}

//Shift right (arithmetic) a register
void CPU::SRA_R(unsigned char* reg8)
{
    instruction = offset("sra " + regp_to_str(reg8));
    short out = *reg8 >> 1;
    //zero flag
    condflag((out & 0xFF) == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(out > 0xFF, CARRY);
    *reg8 = out & 0xFF;
    pc++;
    m = 2;
}

//Shift right (arithmetic) the memory location pointed at by HL
void CPU::SRA_MEM()
{
    unsigned char value = mmu.r8((h << 8) + l);
    SRA_R(&value);
    instruction = offset("sra (hl)");
    mmu.w8(value, (h << 8) + l);
    m = 4;
}

//Swap upper and lower 4 bits of a register
void CPU::SWAP_R(unsigned char* reg8)
{
    instruction = offset("swap " + regp_to_str(reg8));
    *reg8 = ((*reg8 & 0xF) << 4) + ((*reg8 & 0xF0) >> 4);
    //zero flag
    condflag(*reg8 == 0, ZERO);
    //clear all other flags
    clearflag(SUB);
    clearflag(HALF);
    clearflag(CARRY);
    pc++;
    m = 2;
}

//Swap upper and lower 4 bits of the memory address pointed at by HL
void CPU::SWAP_MEM()
{
    unsigned char value = mmu.r8((h << 8) + l);
    SWAP_R(&value);
    instruction = offset("swap (hl)");
    mmu.w8(value, (h << 8) + l);
    m = 4;
}

//Shift right (logical) a register
void CPU::SRL_R(unsigned char* reg8)
{
    instruction = offset("srl " + regp_to_str(reg8));
    short value = (*reg8 >> 1) + ((b & 1) << 8);
    //zero flag
    condflag(value == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    clearflag(HALF);
    //carry flag
    condflag(value > 0xFF, CARRY);
    *reg8 = value & 0xFF;
    pc++;
    m = 2;
}

//Shift right (logical) a memory address
void CPU::SRL_MEM()
{
    instruction = offset("srl (hl)");
    unsigned char value = mmu.r8((h << 8) + l);
    SRL_R(&value);
    mmu.w8(value, (h << 8) + l);
    m = 4;
}

//Check bit N of a register
void CPU::BIT_N_R(char n, unsigned char* reg8)
{
    if (n < 0 || n > 7)
    {
        std::cout << "CPU: Cannot check bit " << std::to_string(int(n)) << " of a register! (CB+Bit Check)" << std::endl;
        throw 1;
    }
    instruction = offset("bit " + std::to_string(int(n)) + ", " + regp_to_str(reg8));
    unsigned char value = *reg8 & (1 << n);
    //zero flag
    condflag(value == 0, ZERO);
    //subtraction flag
    clearflag(SUB);
    //half-carry flag
    setflag(HALF);
    pc++;
    m = 2;
}

//Check bit N of the memory address pointed at by HL
void CPU::BIT_N_MEM(char n)
{
    if (n < 0 || n > 7)
    {
        std::cout << "CPU: Cannot check bit " << std::to_string(int(n)) << " of a memory address! (0xCB Bit Check)" << std::endl;
        throw 1;
    }
    unsigned char value = mmu.r8((h << 8) + l);
    BIT_N_R(n, &value);
    instruction = offset("bit " + std::to_string(int(n)) + ", (hl)");
    mmu.w8(value, (h << 8) + l);
    m = 3;
}

//Reset bit N of a register
void CPU::RES_N_R(char n, unsigned char* reg8)
{
    if (n < 0 || n > 7)
    {
        std::cout << "CPU: Cannot reset bit " << std::to_string(int(n)) << " of a register! (0xCB Bit Reset)" << std::endl;
        throw 1;
    }
    instruction = offset("res " + std::to_string(int(n)) + ", "+ regp_to_str(reg8));
    *reg8 &= ~(1 << n);
    pc++;
    m = 2;
}

//Reset bit N of the memory address pointed at by HL
void CPU::RES_N_MEM(char n)
{
    if (n < 0 || n > 7)
    {
        std::cout << "CPU: Cannot reset bit " << std::to_string(int(n)) << " of a memory address! (0xCB Bit Reset)" << std::endl;
        throw 1;
    }
    unsigned char value = mmu.r8((h << 8) + l);
    RES_N_R(n, &value);
    instruction = offset("res " + std::to_string(int(n)) + ", (hl)");
    m = 4;
}

//Set bit N of a register
void CPU::SET_N_R(char n, unsigned char* reg8)
{
    if (n < 0 || n > 7)
    {
        std::cout << "CPU: Cannot set bit " << std::to_string(int(n)) << " of a register! (0xCB Bit Set)" << std::endl;
        throw 1;
    }
    instruction = offset("set " + std::to_string(int(n)) + + ", " + regp_to_str(reg8));
    *reg8 |= (1 << n);
    pc++;
    m = 2;
}

//Set bit N of the memory address pointed at by HL
void CPU::SET_N_MEM(char n)
{
    if (n < 0 || n > 7)
    {
        std::cout << "CPU: Cannot set bit " << std::to_string(int(n)) << " of a memory address! (0xCB Bit Set)" << std::endl;
        throw 1;
    }
    unsigned char value = mmu.r8((h << 8) + l);
    SET_N_R(n, &value);
    instruction = offset("set " + std::to_string(int(n)) + ", (hl)");
    m = 4;
}

//Bad opcode
void CPU::BAD_CODE(unsigned char code)
{
    std::cout << "CPU: Undefined opcode 0x" << std::hex << int(code) << std::endl;
    std::cout << std::dec;
    throw 1;
}

//Bad prefix opcode
void CPU::BAD_CODE_PREFIX(unsigned char code)
{
    std::cout << "CPU: Undefined opcode 0xcb 0x" << std::hex << code << std::endl;
    std::cout << std::dec;
    throw 1;
}