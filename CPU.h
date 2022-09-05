#pragma once
#include "MMU.h"
enum Flag
{
    ZERO,
    SUB,
    HALF,
    CARRY
};

enum IntType
{
    VB,
    LCDSTAT,
    TIMER,
    SERIAL,
    JOYPAD
};

class CPU
{
    public:
        //halt requested
        bool halt;
        //total M-time
        unsigned char tm;
        //8-bit registers
        unsigned char a;
        unsigned char b;
        unsigned char c;
        unsigned char d;
        unsigned char e;
        unsigned char f;
        unsigned char h;
        unsigned char l;
        //16-bit registers
        unsigned short pc;
        unsigned short sp;
        //last instruction time
        unsigned char m;
        unsigned char t;
        CPU();
        CPU(std::string & rompath);
        void rrfrs();
        void wrtrs();
        void reset();
        //helper methods
        void setflag(Flag flag);
        void clearflag(Flag flag);
        void condflag(bool condition, Flag flag);
        bool checkflag(Flag flag);
        char abs(char in);
        //execute instruction
        void do_op();
        //current instruction
        std::string instruction;
        //instruction format
        std::string offset(std::string badstring);
        std::string ushort_to_hex(unsigned short value);
        std::string uchar_to_hex(unsigned char value);
        std::string dregp_to_str(unsigned char* ureg);
        std::string regp_to_str(unsigned char* reg8);
        std::string flag_to_str(Flag flag);
        //MMU
        MMU mmu;
        //Blargg debug methods
        void serial_update();
        void serial_print();
        //debug message
        char* d_message;
        int msgsize;
        //interrupt master enable
        bool ime;
        //request interrupt
        void request_interrupt(IntType interrupt);
        //step cpu
        void step();
        //kill cpu
        bool die;
    private:
        //cb prefix boolean, for do_op
        bool prefix;
        //reserve copies of registers for memory recall
        unsigned char ra;
        unsigned char rb;
        unsigned char rc;
        unsigned char rd;
        unsigned char re;
        unsigned char rf;
        unsigned char rh;
        unsigned char rl;
        //operations
        void NOP();
        void LD_DR_IMM16(unsigned char* ureg, unsigned char* lreg);
        void LD_SP_IMM16();
        void STR_DR_R(unsigned char* ureg, unsigned char* lreg, unsigned char* reg8);
        void STR_MEMPLUS_R();
        void STR_MEMMINUS_R();
        void INC_DR(unsigned char* ureg, unsigned char* lreg);
        void INC_SP();
        void INC_R(unsigned char* reg8);
        void INC_MEM();
        void DEC_R(unsigned char* reg8);
        void DEC_MEM();
        void LD_R_IMM8(unsigned char* reg8);
        void LD_MEM_IMM8();
        void RLC();
        void RL();
        void STR_IMM16_SP();
        void ADD_DR_DR(unsigned char* ureg, unsigned char* lreg);
        void ADD_DR_SP();
        void LD_R_DR(unsigned char* ureg, unsigned char* lreg);
        void LD_R_MEMPLUS();
        void LD_R_MEMMINUS();
        void DEC_DR(unsigned char* ureg, unsigned char* lreg);
        void DEC_SP();
        void RRC();
        void RR();
        void STOP();
        void JUMP_IMMS8();
        void JUMP_FLAG_IMMS8(Flag flag);
        void JUMP_NFLAG_IMMS8(Flag flag);
        void DAA();
        void CPL();
        void SCF();
        void CCF();
        void LD_R_R(unsigned char* reg1, unsigned char* reg2);
        void LD_R_MEM(unsigned char* reg8);
        void STR_MEM_R(unsigned char* reg8);
        void HALT();
        void ADD_A_R(unsigned char* reg1);
        void ADD_A_MEM();
        void SUB_A_R(unsigned char* reg1);
        void SUB_A_MEM();
        void ADC_A_R(unsigned char* reg1);
        void ADC_A_MEM();
        void SBC_A_R(unsigned char* reg1);
        void SBC_A_MEM();
        void AND_R(unsigned char* reg1);
        void AND_MEM();
        void OR_R(unsigned char* reg1);
        void OR_MEM();
        void XOR_R(unsigned char* reg1);
        void XOR_MEM();
        void CP_R(unsigned char* reg1);
        void CP_MEM();
        void RET_NFLAG(Flag flag);
        void RET_FLAG(Flag flag);
        void POP_DR(unsigned char* ureg, unsigned char* lreg);
        void POP_AF();
        void PUSH_DR(unsigned char* ureg, unsigned char* lreg);
        void JUMP_NFLAG_IMM16(Flag flag);
        void JUMP_FLAG_IMM16(Flag flag);
        void JUMP_IMM16();
        void CALL_NFLAG_IMM16(Flag flag);
        void CALL_FLAG_IMM16(Flag flag);
        void RST_HEX8(unsigned char hex8);
        void JUMP_HL();
        void RET();
        void RETI();
        void CALL_IMM16();
        void LDO_IMM8();
        void STRO_IMM8();
        void LDO_C();
        void STRO_C();
        void LD_IMM16();
        void STR_IMM16();
        void ADD_SP_IMM8();
        void LD_SP_HL();
        void LD_HL_SP_PLUS_IMM8();
        void ADD_IMM8();
        void SUB_IMM8();
        void AND_IMM8();
        void OR_IMM8();
        void ADC_IMM8();
        void SBC_IMM8();
        void XOR_IMM8();
        void CP_IMM8();
        void PREFIX();
        void DI();
        void EI();
        //prefix operations
        void RLC_R(unsigned char* reg8);
        void RLC_MEM();
        void RL_R(unsigned char* reg8);
        void RL_MEM();
        void RRC_R(unsigned char* reg8);
        void RRC_MEM();
        void RR_R(unsigned char* reg8);
        void RR_MEM();
        void SLA_R(unsigned char* reg8);
        void SLA_MEM();
        void SRA_R(unsigned char* reg8);
        void SRA_MEM();
        void SWAP_R(unsigned char* reg8);
        void SWAP_MEM();
        void SRL_R(unsigned char* reg8);
        void SRL_MEM();
        void BIT_N_R(char n, unsigned char* reg8);
        void BIT_N_MEM(char n);
        void RES_N_R(char n, unsigned char* reg8);
        void RES_N_MEM(char n);
        void SET_N_R(char n, unsigned char* reg8);
        void SET_N_MEM(char n);
        //undefined opcode
        void BAD_CODE(unsigned char code);
        void BAD_CODE_PREFIX(unsigned char code);
};