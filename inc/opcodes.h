#ifndef OPCODES_H
#define OPCODES_H

#define OPERATOR_COUNT 56

// Index in opcode table for addressing modes
enum {
    INDEX_ACC,
    INDEX_ABS,
    INDEX_ABX,
    INDEX_ABY,
    INDEX_IMM,
    INDEX_IMP,
    INDEX_IND,
    INDEX_INX,
    INDEX_INY,
    INDEX_REL,
    INDEX_ZPG,
    INDEX_ZPX,
    INDEX_ZPY,
    AM_COUNT
};

// Mask to tell wether an instruction is legal with a certain addressing mode
enum {
    LEGAL_MASK_ACC = 0x0001,
    LEGAL_MASK_ABS = 0x0002,
    LEGAL_MASK_ABX = 0x0004,
    LEGAL_MASK_ABY = 0x0008,
    LEGAL_MASK_IMM = 0x0010,
    LEGAL_MASK_IMP = 0x0020,
    LEGAL_MASK_IND = 0x0040,
    LEGAL_MASK_INX = 0x0080,
    LEGAL_MASK_INY = 0x0100,
    LEGAL_MASK_REL = 0x0200,
    LEGAL_MASK_ZPG = 0x0400,
    LEGAL_MASK_ZPX = 0x0800,
    LEGAL_MASK_ZPY = 0x1000
};

// Struct which holds information about a particular instruction
typedef struct {
    const char name[4];
    const unsigned char codes[AM_COUNT];
    const unsigned short legal_mask;
} instruction_data_t;

extern instruction_data_t instruction_data_table[OPERATOR_COUNT];

unsigned char hash(const char *str);

#endif