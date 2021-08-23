#ifndef TOKEN_H
#define TOKEN_H

// A token type is a mask
typedef enum {
    TOKEN_INVALID     = 0x01,
    TOKEN_INSTRUCTION = 0x02,
    TOKEN_NUMBER      = 0x04,
    TOKEN_DIRECTIVE   = 0x08,
    TOKEN_EOF         = 0x10,
} token_types;

typedef struct {
    token_types type;
    unsigned long id;

    union {
        unsigned char instruction_index;
        struct {
            unsigned short value;
            unsigned short possible_addressing_modes;
        } number_attrib;
        unsigned char directive_type;
    } data;
} token_t;

#endif