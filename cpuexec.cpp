#include "CPU.h"
#include <iostream>

//execute an instruction
void CPU::do_op()
{
    unsigned char code = mmu.r8(pc);
    //std::cout << "0x" << uchar_to_hex(code) << ": ";
    if (!prefix)
    {
        switch (code)
        {
            case 0x00:
                NOP();
                break;
            case 0x01:
                LD_DR_IMM16(&b, &c);
                break;
            case 0x02:
                STR_DR_R(&b, &c, &a);
                break;
            case 0x03:
                INC_DR(&b, &c);
                break;
            case 0x04:
                INC_R(&b);
                break;
            case 0x05:
                DEC_R(&b);
                break;
            case 0x06:
                LD_R_IMM8(&b);
                break;
            case 0x07:
                RLC();
                break;
            case 0x08:
                LD_SP_IMM16();
                break;
            case 0x09:
                ADD_DR_DR(&b, &c);
                break;
            case 0x0A:
                LD_R_DR(&b, &c);
                break;
            case 0x0B:
                DEC_DR(&b, &c);
                break;
            case 0x0C:
                INC_R(&c);
                break;
            case 0x0D:
                DEC_R(&c);
                break;
            case 0x0E:
                LD_R_IMM8(&c);
                break;
            case 0x0F:
                RRC();
                break;
            case 0x10:
                STOP();
                break;
            case 0x11:
                LD_DR_IMM16(&d, &e);
                break;
            case 0x12:
                STR_DR_R(&d, &e, &a);
                break;
            case 0x13:
                INC_DR(&d, &e);
                break;
            case 0x14:
                INC_R(&d);
                break;
            case 0x15:
                DEC_R(&e);
                break;
            case 0x16:
                LD_R_IMM8(&d);
                break;
            case 0x17:
                RL();
                break;
            case 0x18:
                JUMP_IMMS8();
                break;
            case 0x19:
                ADD_DR_DR(&d, &e);
                break;
            case 0x1A:
                LD_R_DR(&d, &e);
                break;
            case 0x1B:
                DEC_DR(&d, &e);
                break;
            case 0x1C:
                INC_R(&e);
                break;
            case 0x1D:
                DEC_R(&e);
                break;
            case 0x1E:
                LD_R_IMM8(&e);
                break;
            case 0x1F:
                RR();
                break;
            case 0x20:
                JUMP_NFLAG_IMMS8(ZERO);
                break;
            case 0x21:
                LD_DR_IMM16(&h, &l);
                break;
            case 0x22:
                STR_MEMPLUS_R();
                break;
            case 0x23:
                INC_DR(&h, &l);
                break;
            case 0x24:
                INC_R(&h);
                break;
            case 0x25:
                DEC_R(&h);
                break;
            case 0x26:
                LD_R_IMM8(&h);
                break;
            case 0x27:
                DAA();
                break;
            case 0x28:
                JUMP_FLAG_IMMS8(ZERO);
                break;
            case 0x29:
                ADD_DR_DR(&h, &l);
                break;
            case 0x2A:
                LD_R_MEMPLUS();
                break;
            case 0x2B:
                DEC_DR(&h, &l);
                break;
            case 0x2C:
                INC_R(&l);
                break;
            case 0x2D:
                DEC_R(&l);
                break;
            case 0x2E:
                LD_R_IMM8(&l);
                break;
            case 0x2F:
                CPL();
                break;
            case 0x30:
                JUMP_NFLAG_IMMS8(CARRY);
                break;
            case 0x31:
                LD_SP_IMM16();
                break;
            case 0x32:
                STR_MEMMINUS_R();
                break;
            case 0x33:
                INC_SP();
                break;
            case 0x34:
                INC_MEM();
                break;
            case 0x35:
                DEC_MEM();
                break;
            case 0x36:
                LD_MEM_IMM8();
                break;
            case 0x37:
                SCF();
                break;
            case 0x38:
                JUMP_FLAG_IMMS8(CARRY);
                break;
            case 0x39:
                ADD_DR_SP();
                break;
            case 0x3A:
                LD_R_MEMMINUS();
                break;
            case 0x3B:
                DEC_SP();
                break;
            case 0x3C:
                INC_R(&a);
                break;
            case 0x3D:
                DEC_R(&a);
                break;
            case 0x3E:
                LD_R_IMM8(&a);
                break;
            case 0x3F:
                CCF();
                break;
            case 0x40:
                LD_R_R(&b, &b);
                break;
            case 0x41:
                LD_R_R(&b, &c);
                break;
            case 0x42:
                LD_R_R(&b, &d);
                break;
            case 0x43:
                LD_R_R(&b, &e);
                break;
            case 0x44:
                LD_R_R(&b, &h);
                break;
            case 0x45:
                LD_R_R(&b, &l);
                break;
            case 0x46:
                LD_R_MEM(&b);
                break;
            case 0x47:
                LD_R_R(&b, &a);
                break;
            case 0x48:
                LD_R_R(&c, &b);
                break;
            case 0x49:
                LD_R_R(&c, &c);
                break;
            case 0x4A:
                LD_R_R(&c, &d);
                break;
            case 0x4B:
                LD_R_R(&c, &e);
                break;
            case 0x4C:
                LD_R_R(&c, &h);
                break;
            case 0x4D:
                LD_R_R(&c, &l);
                break;
            case 0x4E:
                LD_R_MEM(&c);
                break;
            case 0x4F:
                LD_R_R(&c, &a);
                break;
            case 0x50:
                LD_R_R(&d, &b);
                break;
            case 0x51:
                LD_R_R(&d, &c);
                break;
            case 0x52:
                LD_R_R(&d, &d);
                break;
            case 0x53:
                LD_R_R(&d, &e);
                break;
            case 0x54:
                LD_R_R(&d, &h);
                break;
            case 0x55:
                LD_R_R(&d, &l);
                break;
            case 0x56:
                LD_R_MEM(&d);
                break;
            case 0x57:
                LD_R_R(&d, &a);
                break;
            case 0x58:
                LD_R_R(&e, &b);
                break;
            case 0x59:
                LD_R_R(&e, &c);
                break;
            case 0x5A:
                LD_R_R(&e, &d);
                break;
            case 0x5B:
                LD_R_R(&e, &e);
                break;
            case 0x5C:
                LD_R_R(&e, &h);
                break;
            case 0x5D:
                LD_R_R(&e, &l);
                break;
            case 0x5E:
                LD_R_MEM(&e);
                break;
            case 0x5F:
                LD_R_R(&e, &a);
                break;
            case 0x60:
                LD_R_R(&h, &b);
                break;
            case 0x61:
                LD_R_R(&h, &c);
                break;
            case 0x62:
                LD_R_R(&h, &d);
                break;
            case 0x63:
                LD_R_R(&h, &e);
                break;
            case 0x64:
                LD_R_R(&h, &h);
                break;
            case 0x65:
                LD_R_R(&h, &l);
                break;
            case 0x66:
                LD_R_MEM(&h);
                break;
            case 0x67:
                LD_R_R(&h, &a);
                break;
            case 0x68:
                LD_R_R(&l, &b);
                break;
            case 0x69:
                LD_R_R(&l, &c);
                break;
            case 0x6A:
                LD_R_R(&l, &d);
                break;
            case 0x6B:
                LD_R_R(&l, &e);
                break;
            case 0x6C:
                LD_R_R(&l, &h);
                break;
            case 0x6D:
                LD_R_R(&l, &l);
                break;
            case 0x6E:
                LD_R_MEM(&l);
                break;
            case 0x6F:
                LD_R_R(&l, &a);
                break;
            case 0x70:
                STR_MEM_R(&b);
                break;
            case 0x71:
                STR_MEM_R(&c);
                break;
            case 0x72:
                STR_MEM_R(&d);
                break;
            case 0x73:
                STR_MEM_R(&e);
                break;
            case 0x74:
                STR_MEM_R(&h);
                break;
            case 0x75:
                STR_MEM_R(&l);
                break;
            case 0x76:
                HALT();
                break;
            case 0x77:
                STR_MEM_R(&a);
                break;
            case 0x78:
                LD_R_R(&a, &b);
                break;
            case 0x79:
                LD_R_R(&a, &c);
                break;
            case 0x7A:
                LD_R_R(&a, &d);
                break;
            case 0x7B:
                LD_R_R(&a, &e);
                break;
            case 0x7C:
                LD_R_R(&a, &h);
                break;
            case 0x7D:
                LD_R_R(&a, &l);
                break;
            case 0x7E:
                LD_R_MEM(&a);
                break;
            case 0x7F:
                LD_R_R(&a, &a);
                break;
            case 0x80:
                ADD_A_R(&b);
                break;
            case 0x81:
                ADD_A_R(&c);
                break;
            case 0x82:
                ADD_A_R(&d);
                break;
            case 0x83:
                ADD_A_R(&e);
                break;
            case 0x84:
                ADD_A_R(&h);
                break;
            case 0x85:
                ADD_A_R(&l);
                break;
            case 0x86:
                ADD_A_MEM();
                break;
            case 0x87:
                ADD_A_R(&a);
                break;
            case 0x88:
                ADC_A_R(&b);
                break;
            case 0x89:
                ADC_A_R(&c);
                break;
            case 0x8A:
                ADC_A_R(&d);
                break;
            case 0x8B:
                ADC_A_R(&e);
                break;
            case 0x8C:
                ADC_A_R(&h);
                break;
            case 0x8D:
                ADC_A_R(&l);
                break;
            case 0x8E:
                ADC_A_MEM();
                break;
            case 0x8F:
                ADC_A_R(&a);
                break;
            case 0x90:
                SUB_A_R(&b);
                break;
            case 0x91:
                SUB_A_R(&c);
                break;
            case 0x92:
                SUB_A_R(&d);
                break;
            case 0x93:
                SUB_A_R(&e);
                break;
            case 0x94:
                SUB_A_R(&h);
                break;
            case 0x95:
                SUB_A_R(&l);
                break;
            case 0x96:
                SUB_A_MEM();
                break;
            case 0x97:
                SUB_A_R(&a);
                break;
            case 0x98:
                SBC_A_R(&b);
                break;
            case 0x99:
                SBC_A_R(&c);
                break;
            case 0x9A:
                SBC_A_R(&d);
                break;
            case 0x9B:
                SBC_A_R(&e);
                break;
            case 0x9C:
                SBC_A_R(&h);
                break;
            case 0x9D:
                SBC_A_R(&l);
                break;
            case 0x9E:
                SBC_A_MEM();
                break;
            case 0x9F:
                SBC_A_R(&a);
                break;
            case 0xA0:
                AND_R(&b);
                break;
            case 0xA1:
                AND_R(&c);
                break;
            case 0xA2:
                AND_R(&d);
                break;
            case 0xA3:
                AND_R(&e);
                break;
            case 0xA4:
                AND_R(&h);
                break;
            case 0xA5:
                AND_R(&l);
                break;
            case 0xA6:
                AND_MEM();
                break;
            case 0xA7:
                AND_R(&a);
                break;
            case 0xA8:
                XOR_R(&b);
                break;
            case 0xA9:
                XOR_R(&c);
                break;
            case 0xAA:
                XOR_R(&d);
                break;
            case 0xAB:
                XOR_R(&e);
                break;
            case 0xAC:
                XOR_R(&h);
                break;
            case 0xAD:
                XOR_R(&l);
                break;
            case 0xAE:
                XOR_MEM();
                break;
            case 0xAF:
                XOR_R(&a);
                break;
            case 0xB0:
                OR_R(&b);
                break;
            case 0xB1:
                OR_R(&c);
                break;
            case 0xB2:
                OR_R(&d);
                break;
            case 0xB3:
                OR_R(&e);
                break;
            case 0xB4:
                OR_R(&h);
                break;
            case 0xB5:
                OR_R(&l);
                break;
            case 0xB6:
                OR_MEM();
                break;
            case 0xB7:
                OR_R(&a);
                break;
            case 0xB8:
                CP_R(&b);
                break;
            case 0xB9:
                CP_R(&c);
                break;
            case 0xBA:
                CP_R(&d);
                break;
            case 0xBB:
                CP_R(&e);
                break;
            case 0xBC:
                CP_R(&h);
                break;
            case 0xBD:
                CP_R(&l);
                break;
            case 0xBE:
                CP_MEM();
                break;
            case 0xBF:
                CP_R(&a);
                break;
            case 0xC0:
                RET_NFLAG(ZERO);
                break;
            case 0xC1:
                POP_DR(&b, &c);
                break;
            case 0xC2:
                JUMP_NFLAG_IMM16(ZERO);
                break;
            case 0xC3:
                JUMP_IMM16();
                break;
            case 0xC4:
                CALL_NFLAG_IMM16(ZERO);
                break;
            case 0xC5:
                PUSH_DR(&b, &c);
                break;
            case 0xC6:
                ADD_IMM8();
                break;
            case 0xC7:
                RST_HEX8(0x00);
                break;
            case 0xC8:
                RET_FLAG(ZERO);
                break;
            case 0xC9:
                RET();
                break;
            case 0xCA:
                JUMP_FLAG_IMM16(ZERO);
                break;
            case 0xCB:
                PREFIX();
                break;
            case 0xCC:
                CALL_FLAG_IMM16(ZERO);
                break;
            case 0xCD:
                CALL_IMM16();
                break;
            case 0xCE:
                ADC_IMM8();
                break;
            case 0xCF:
                RST_HEX8(0x08);
                break;
            case 0xD0:
                RET_NFLAG(CARRY);
                break;
            case 0xD1:
                POP_DR(&d, &e);
                break;
            case 0xD2:
                JUMP_NFLAG_IMM16(CARRY);
                break;
            case 0xD3:
                BAD_CODE(code);
                break;
            case 0xD4:
                CALL_NFLAG_IMM16(CARRY);
                break;
            case 0xD5:
                PUSH_DR(&d, &e);
                break;
            case 0xD6:
                SUB_IMM8();
                break;
            case 0xD7:
                RST_HEX8(0x10);
                break;
            case 0xD8:
                RET_FLAG(CARRY);
                break;
            case 0xD9:
                RETI();
                break;
            case 0xDA:
                JUMP_FLAG_IMM16(CARRY);
                break;
            case 0xDB:
                BAD_CODE(code);
                break;
            case 0xDC:
                CALL_FLAG_IMM16(CARRY);
                break;
            case 0xDD:
                BAD_CODE(code);
                break;
            case 0xDE:
                SBC_IMM8();
                break;
            case 0xDF:
                RST_HEX8(0x18);
                break;
            case 0xE0:
                STRO_IMM8();
                break;
            case 0xE1:
                POP_DR(&h, &l);
                break;
            case 0xE2:
                STRO_C();
                break;
            case 0xE3:
                BAD_CODE(code);
                break;
            case 0xE4:
                BAD_CODE(code);
                break;
            case 0xE5:
                PUSH_DR(&h, &l);
                break;
            case 0xE6:
                AND_IMM8();
                break;
            case 0xE7:
                RST_HEX8(0x20);
                break;
            case 0xE8:
                ADD_SP_IMM8();
                break;
            case 0xE9:
                JUMP_HL();
                break;
            case 0xEA:
                LD_IMM16();
                break;
            case 0xEB:
                BAD_CODE(code);
                break;
            case 0xEC:
                BAD_CODE(code);
                break;
            case 0xED:
                BAD_CODE(code);
                break;
            case 0xEE:
                XOR_IMM8();
                break;
            case 0xEF:
                RST_HEX8(0x28);
                break;
            case 0xF0:
                LDO_IMM8();
                break;
            case 0xF1:
                POP_AF();
                break;
            case 0xF2:
                LDO_C();
                break;
            case 0xF3:
                DI();
                break;
            case 0xF4:
                BAD_CODE(code);
                break;
            case 0xF5:
                PUSH_DR(&a, &f);
                break;
            case 0xF6:
                OR_IMM8();
                break;
            case 0xF7:
                RST_HEX8(0x30);
                break;
            case 0xF8:
                LD_HL_SP_PLUS_IMM8();
                break;
            case 0xF9:
                LD_SP_HL();
                break;
            case 0xFA:
                LD_IMM16();
                break;
            case 0xFB:
                EI();
                break;
            case 0xFC:
                BAD_CODE(code);
                break;
            case 0xFD:
                BAD_CODE(code);
                break;
            case 0xFE:
                CP_IMM8();
                break;
            case 0xFF:
                RST_HEX8(0x38);
                break;
            default:
                BAD_CODE(code);
                break;
        }
    }
    else
    {
        unsigned char* rpointers[] = {&b, &c, &d, &e, &h, &l, nullptr, &a};
        int bottom4 = code & 0x0F;
        switch (code & 0xF0)
        {
            //RLC, RRC
            case 0x00:
                if (bottom4 < 0x08)
                {
                    if (bottom4 != 0x06)
                        RLC_R(rpointers[bottom4]);
                    else
                        RLC_MEM();
                }
                else
                {
                    if ((bottom4 - 0x08) != 0x06)
                        RRC_R(rpointers[bottom4 - 0x08]);
                    else
                        RRC_MEM();
                }
                break;
            //RL, RR
            case 0x10:
                if (bottom4 < 0x08)
                {
                    if (bottom4 != 0x06)
                        RL_R(rpointers[bottom4]);
                    else
                        RL_MEM();
                }
                else
                {
                    if ((bottom4 - 0x08) != 0x06)
                        RR_R(rpointers[bottom4 - 0x08]);
                    else
                        RR_MEM();
                }
                break;
            //SLA, SRA
            case 0x20:
                if (bottom4 < 0x08)
                {
                    if (bottom4 != 0x06)
                        SLA_R(rpointers[bottom4]);
                    else
                        SLA_MEM();
                }
                else
                {
                    if ((bottom4 - 0x08) != 0x06)
                        SRA_R(rpointers[bottom4 - 0x08]);
                    else
                        SRA_MEM();
                }
                break;
            //SWAP, SRL
            case 0x30:
                if (bottom4 < 0x08)
                {
                    if (bottom4 != 0x06)
                        SWAP_R(rpointers[bottom4]);
                    else
                        SWAP_MEM();
                }
                else
                {
                    if ((bottom4 - 0x08) != 0x06)
                        SRL_R(rpointers[bottom4 - 0x08]);
                    else
                        SRL_MEM();
                }
                break;
            //BIT_N_R
            case 0x40:
            case 0x50:
            case 0x60:
            case 0x70:
                {
                    char bit = (((code & 0xF0) - 0x40) / 8);
                    int index = bottom4 % 0x08;
                    if (bottom4 > 0x07)
                        bit++;
                    if (index != 0x06)
                        BIT_N_R(bit, rpointers[index]);
                    else
                        BIT_N_MEM(bit);
                    break;
                }
            //RES_N_R
            case 0x80:
            case 0x90:
            case 0xA0:
            case 0xB0:
                {
                    char bit = (((code & 0xF0) - 0x80) / 8);
                    int index = bottom4 % 0x08;
                    if (bottom4 > 0x07)
                        bit++;
                    if (index != 0x06)
                        RES_N_R(bit, rpointers[index]);
                    else
                        RES_N_MEM(bit);
                    break;
                }
            //SET_N_R
            case 0xC0:
            case 0xD0:
            case 0xE0:
            case 0xF0:
                {
                    char bit = (((code & 0xF0) - 0xC0) / 8);
                    int index = bottom4 % 0x08;
                    if (bottom4 > 0x07)
                        bit++;
                    if (index != 0x06)
                        BIT_N_R(bit, rpointers[index]);
                    else
                        BIT_N_MEM(bit);
                    break;
                }
            default:
                BAD_CODE_PREFIX(code);
        }
        prefix = false;
    }
    tm += m * 4;
}