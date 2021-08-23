#include "opcodes.h"

#include <string.h>

#define OP(name, acc, imm, zpg, zpx, zpy, abs, abx, aby, ind, inx, iny, imp, rel) \
    { #name, \
    { 0x##acc, 0x##abs, 0x##abx, 0x##aby, 0x##imm, 0x##imp, 0x##ind, 0x##inx, 0x##iny, 0x##rel, 0x##zpg, 0x##zpx, 0x##zpy}, \
        ((0x##acc != 0) * LEGAL_MASK_ACC) | \
        ((0x##imm != 0) * LEGAL_MASK_IMM) | \
        ((0x##zpg != 0) * LEGAL_MASK_ZPG) | \
        ((0x##zpx != 0) * LEGAL_MASK_ZPX) | \
        ((0x##zpy != 0) * LEGAL_MASK_ZPY) | \
        ((0x##abs != 0) * LEGAL_MASK_ABS) | \
        ((0x##abx != 0) * LEGAL_MASK_ABX) | \
        ((0x##aby != 0) * LEGAL_MASK_ABY) | \
        ((0x##ind != 0) * LEGAL_MASK_IND) | \
        ((0x##inx != 0) * LEGAL_MASK_INX) | \
        ((0x##iny != 0) * LEGAL_MASK_INY) | \
        ((0x##imp != 0) * LEGAL_MASK_IMP) | \
        ((0x##rel != 0) * LEGAL_MASK_REL) \
    }

#define BRK {"BRK",{0,0,0,0,0,0,0,0,0,0,0,0,0},LEGAL_MASK_IMP}

instruction_data_t instruction_data_table[OPERATOR_COUNT] = {
    OP(ADC,00,69,65,75,00,6D,7D,79,00,61,71,00,00),
    OP(AND,00,29,25,35,00,2D,3D,39,00,21,31,00,00),
    OP(ASL,0A,00,06,16,00,0E,1E,00,00,00,00,00,00),
    OP(BCC,00,00,00,00,00,00,00,00,00,00,00,00,90),
    OP(BCS,00,00,00,00,00,00,00,00,00,00,00,00,B0),
    OP(BEQ,00,00,00,00,00,00,00,00,00,00,00,00,F0),
    OP(BIT,00,00,24,00,00,2C,00,00,00,00,00,00,00),
    OP(BMI,00,00,00,00,00,00,00,00,00,00,00,00,30),
    OP(BNE,00,00,00,00,00,00,00,00,00,00,00,00,D0),
    OP(BPL,00,00,00,00,00,00,00,00,00,00,00,00,10),
    BRK,
    OP(BVC,00,00,00,00,00,00,00,00,00,00,00,00,50),
    OP(BVS,00,00,00,00,00,00,00,00,00,00,00,00,70),
    OP(CLC,00,00,00,00,00,00,00,00,00,00,00,18,00),
    OP(CLD,00,00,00,00,00,00,00,00,00,00,00,D8,00),
    OP(CLI,00,00,00,00,00,00,00,00,00,00,00,58,00),
    OP(CLV,00,00,00,00,00,00,00,00,00,00,00,B8,00),
    OP(CMP,00,C9,C5,D5,00,CD,DD,D9,00,C1,D1,00,00),
    OP(CPX,00,E0,E4,00,00,EC,00,00,00,00,00,00,00),
    OP(CPY,00,C0,C4,00,00,CC,00,00,00,00,00,00,00),
    OP(DEC,00,00,C6,D6,00,CE,DE,00,00,00,00,00,00),
    OP(DEX,00,00,00,00,00,00,00,00,00,00,00,CA,00),
    OP(DEY,00,00,00,00,00,00,00,00,00,00,00,88,00),
    OP(EOR,00,49,45,55,00,4D,5D,59,00,41,51,00,00),
    OP(INC,00,00,E6,F6,00,EE,FE,00,00,00,00,00,00),
    OP(INX,00,00,00,00,00,00,00,00,00,00,00,E8,00),
    OP(INY,00,00,00,00,00,00,00,00,00,00,00,C8,00),
    OP(JMP,00,00,00,00,00,4C,00,00,6C,00,00,00,00),
    OP(JSR,00,00,00,00,00,20,00,00,00,00,00,00,00),
    OP(LDA,00,A9,A5,B5,00,AD,BD,B9,00,A1,B1,00,00),
    OP(LDX,00,A2,A6,00,B6,AE,00,BE,00,00,00,00,00),
    OP(LDY,00,A0,A4,B4,00,AC,BC,00,00,00,00,00,00),
    OP(LSR,4A,00,46,56,00,4E,5E,00,00,00,00,00,00),
    OP(NOP,00,00,00,00,00,00,00,00,00,00,00,EA,00),
    OP(ORA,00,09,05,15,00,0D,1D,19,00,01,11,00,00),
    OP(PHA,00,00,00,00,00,00,00,00,00,00,00,48,00),
    OP(PHP,00,00,00,00,00,00,00,00,00,00,00,08,00),
    OP(PLA,00,00,00,00,00,00,00,00,00,00,00,68,00),
    OP(PLP,00,00,00,00,00,00,00,00,00,00,00,28,00),
    OP(ROL,2A,00,26,36,00,2E,3E,00,00,00,00,00,00),
    OP(ROR,6A,00,66,76,00,6E,7E,00,00,00,00,00,00),
    OP(RTI,00,00,00,00,00,00,00,00,00,00,00,40,00),
    OP(RTS,00,00,00,00,00,00,00,00,00,00,00,60,00),
    OP(SBC,00,ED,FD,F9,E9,00,00,E1,F1,00,E5,F5,00),
    OP(SEC,00,00,00,00,00,38,00,00,00,00,00,00,00),
    OP(SED,00,00,00,00,00,F8,00,00,00,00,00,00,00),
    OP(SEI,00,00,00,00,00,78,00,00,00,00,00,00,00),
    OP(STA,00,8D,9D,99,00,00,00,81,91,00,85,95,00),
    OP(STX,00,8E,00,00,00,00,00,00,00,00,86,00,96),
    OP(STY,00,8C,00,00,00,00,00,00,00,00,84,94,00),
    OP(TAX,00,00,00,00,00,AA,00,00,00,00,00,00,00),
    OP(TAY,00,00,00,00,00,A8,00,00,00,00,00,00,00),
    OP(TSX,00,00,00,00,00,BA,00,00,00,00,00,00,00),
    OP(TXA,00,00,00,00,00,8A,00,00,00,00,00,00,00),
    OP(TXS,00,00,00,00,00,9A,00,00,00,00,00,00,00),
    OP(TYA,00,00,00,00,00,98,00,00,00,00,00,00,00),
};

#undef BRK
#undef OP
#define UNKNOWN_OP 0

unsigned char hash(const char *str) {
    char c1 = str[0] | 0x20;
    char c2 = str[1] | 0x20;
    char c3 = str[2] | 0x20;

    switch (c1) {
        case 'a': 
            switch (c2) {
                case 'd': if (c3 == 'c') return  1; break;
                case 'n': if (c3 == 'd') return  2; break;
                case 's': if (c3 == 'l') return  3; break;
                default: return UNKNOWN_OP;
            }
        break;

        case 'b':
            switch (c2) {
                case 'c':
                    switch (c3) {
                        case 'c':        return  4;
                        case 's':        return  5;
                        default: return UNKNOWN_OP;
                    }
                case 'e': if (c3 == 'q') return  6; break;
                case 'i': if (c3 == 't') return  7; break;
                case 'm': if (c3 == 'i') return  8; break;
                case 'n': if (c3 == 'e') return  9; break;
                case 'p': if (c3 == 'l') return 10; break;
                case 'r': if (c3 == 'k') return 11; break;
                case 'v': 
                    switch (c3) {
                        case 'c':        return 12;
                        case 's':        return 13;
                        default: return UNKNOWN_OP; 
                    }
                default: return UNKNOWN_OP;
            }
        break;

        case 'c':
            switch (c2) {
                case 'l':
                    switch (c3) {
                        case 'c':        return 14; 
                        case 'd':        return 15;
                        case 'i':        return 16;
                        case 'v':        return 17;
                        default: return UNKNOWN_OP;
                    }
                case 'm': if (c3 == 'p') return 18;
                case 'p':
                    switch (c3) {
                        case 'x':        return 19;
                        case 'y':        return 20;
                        default: return UNKNOWN_OP;
                    }
                default: return UNKNOWN_OP;
            }
        break;

        case 'd':
            switch (c2) {
                case 'e':
                    switch (c3) {
                        case 'c':        return 21;
                        case 'x':        return 22;
                        case 'y':        return 23;
                        default: return UNKNOWN_OP;
                    }
                default: return UNKNOWN_OP;
            }
        break;

        case 'e':
            if (c2 == 'o' && c3 == 'r')  return 24;
        break;

        case 'i':
            switch (c2) {
                case 'n':
                    switch (c3) {
                        case 'c':        return 25;
                        case 'x':        return 26;
                        case 'y':        return 27;
                        default: return UNKNOWN_OP;
                    }
                default: return UNKNOWN_OP;
            }
        break;
        
        case 'j':
            switch (c2) {
                case 'm': if (c3 == 'p') return 28; break;
                case 's': if (c3 == 'r') return 29; break;
                default: return UNKNOWN_OP;
            }
        break;
        
        case 'l':
            switch (c2) {
                case 'd':
                    switch (c3) {
                        case 'a':        return 30;
                        case 'x':        return 31;
                        case 'y':        return 32;
                        default: return UNKNOWN_OP;
                    }
                case 's': if (c3 == 'r') return 33; break;
                default: return UNKNOWN_OP;
            }
        break;

        case 'n':
            if (c2 == 'o' && c3 == 'p')  return 34;
        break;

        case 'o':
            if (c2 == 'r' && c3 == 'a')  return 35;
        break;

        case 'p':
            switch (c2) {
                case 'h':
                    switch (c3) {
                        case 'a':        return 36;
                        case 'p':        return 37;
                        default: return UNKNOWN_OP;
                    }
                case 'l':
                    switch (c3) {
                        case 'a':        return 38;
                        case 'p':        return 39;
                        default: return UNKNOWN_OP;
                    }
                default: return UNKNOWN_OP;
            }
        break;

        case 'r':
            switch (c2) {
                case 'o':
                    switch (c3) {
                        case 'l':        return 40;
                        case 'r':        return 41;
                        default: return UNKNOWN_OP;
                    }
                case 't':
                    switch (c3) {
                        case 'i':        return 42;
                        case 's':        return 43;
                        default: return UNKNOWN_OP;
                    }
                default: return UNKNOWN_OP;
            }
        break;

        case 's':
            switch (c2) {
                case 'b': if (c3 == 'c') return 44;
                case 'e':
                    switch (c3) {
                        case 'c':        return 45;
                        case 'd':        return 46;
                        default: return UNKNOWN_OP;
                    }
                case 't':
                    switch (c3) {
                        case 'i':        return 47;
                        case 'a':        return 48;
                        case 'x':        return 49;
                        case 'y':        return 50;
                        default: return UNKNOWN_OP;
                    }
                default: return UNKNOWN_OP;
            }
        break;

        case 't':
            switch (c2) {
                case 'a':
                    switch (c3) {
                        case 'x':        return 51;
                        case 'y':        return 52;
                        default: return UNKNOWN_OP;
                    }
                case 's': if (c3 == 'x') return 53;
                case 'x':
                    switch (c3) {
                        case 'a':        return 54;
                        case 's':        return 55;
                        default: return UNKNOWN_OP;
                    }
                case 'y': if (c3 == 'a') return 56;
                default: return UNKNOWN_OP;
            }
        break;

        default: return UNKNOWN_OP;
    }

    return UNKNOWN_OP;
}