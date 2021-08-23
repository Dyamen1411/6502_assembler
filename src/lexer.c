#include "lexer.h"
#include "opcodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A lexer is a tool to do lexical analysis.
typedef struct {
    char *str;
    unsigned long str_len;
    unsigned long index;
    char current_char;
} internal_lexer_t;

// Tells wether a character is 'white' or not
unsigned char is_white(char c) {
    return (c) == ' ' || (c) == '\t';
}

// Tells wether a character is a delimiter or not
unsigned char is_delim(char c) {
    return is_white(c) || c == '\n' || c == ';' || c == 0;
}

// Tells wether a character is hexadecimal or not
unsigned char is_hex(char c) {
    return ('0' <= c && c <= '9') || ('a' <= (c | 0x20) && (c | 0x20) <= 'f');
}

// Creates a lexer from a string
lexer_t* lexer_create(char *string) {
    internal_lexer_t *lexer = (internal_lexer_t*) malloc(sizeof(internal_lexer_t));
    lexer->str_len = strlen(string);
    lexer->str = string;
    lexer->current_char = *lexer->str;
    lexer->index = 0;
    return (lexer_t*) lexer;
}

// Fetches next character in the lexer
void next(internal_lexer_t *lexer) {
    if (lexer->current_char != 0) {
        lexer->current_char = lexer->str[++lexer->index];
    }
}

// Destroys $lexer
void lexer_delete(lexer_t *lexer) {
    internal_lexer_t *ilexer = (internal_lexer_t*) lexer;
    free(ilexer);
}

// Tells wther $str is represents an ABS value.
// If so, reads value and puts it in $value.
unsigned char is_abs(char *str, short *value) {
    if (strlen(str) < 5) {
        return 0;
    }

    char *value_ptr = str + 1;

    if (*str == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFFFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an ABX value.
// If so, reads value and puts it in $value.
unsigned char is_abx(char *str, short *value) {
    if (strlen(str) < 7) {
        return 0;
    }

    char *value_ptr = str + 1;

    if (*str == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ','
        && (*(++str) | 0x20) == 'x'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFFFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an ABY value.
// If so, reads value and puts it in $value.
unsigned char is_aby(char *str, short *value) {
    if (strlen(str) < 7) {
        return 0;
    }

    char *value_ptr = str + 1;

    if (*str == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ','
        && (*(++str) | 0x20) == 'y'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFFFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an IMM value.
// If so, reads value and puts it in $value.
unsigned char is_imm(char *str, short *value) {
    if (strlen(str) < 4) {
        return 0;
    }

    char *value_ptr = str + 2;

    if (*(str) == '#'
        && *(++str) == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an IND value.
// If so, reads value and puts it in $value.
unsigned char is_ind(char *str, short *value) {
    if (strlen(str) < 7) {
        return 0;
    }

    char *value_ptr = str + 2;

    if (*(str) == '('
        && *(++str) == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ')'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFFFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an INX value.
// If so, reads value and puts it in $value.
unsigned char is_inx(char *str, short *value) {
    if (strlen(str) < 7) {
        return 0;
    }

    char *value_ptr = str + 2;

    if (*(str) == '('
        && *(++str) == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ','
        && (*(++str) | 0x20) == 'x'
        && *(++str) == ')'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an INY value.
// If so, reads value and puts it in $value.
unsigned char is_iny(char *str, short *value) {
    if (strlen(str) < 7) {
        return 0;
    }

    char *value_ptr = str + 2;

    if (*(str) == '('
        && *(++str) == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ')'
        && *(++str) == ','
        && (*(++str) | 0x20) == 'y'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents a REL or ZPG value.
// If so, reads value and puts it in $value.
unsigned char is_rel_zpg(char *str, short *value) {
    if (strlen(str) < 3) {
        return 0;
    }
    char *value_ptr = str + 1;

    if (*str == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFF);
        return 1;
    }

    return 0;
}


// Tells wther $str is represents a ZPG value.
// If so, reads value and puts it in $value.
unsigned char is_zpx(char *str, short *value) {
    if (strlen(str) < 5) {
        return 0;
    }

    char *value_ptr = str + 1;

    if (*str == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ','
        && (*(++str) | 0x20) == 'x'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFF);
        return 1;
    }

    return 0;
}

// Tells wther $str is represents an ZPG value.
// If so, reads value and puts it in $value.
unsigned char is_zpy(char *str, short *value) {
    if (strlen(str) < 5) {
        return 0;
    }

    char *value_ptr = str + 1;

    if (*str == '$'
        && is_hex(*(++str))
        && is_hex(*(++str))
        && *(++str) == ','
        && (*(++str) | 0x20) == 'y'
        && is_delim(*(++str))) {
        *value = (short) (strtol(value_ptr, NULL, 16) & 0xFF);
        return 1;
    }

    return 0;
}

token_t lexer_get_next_token(lexer_t *lexer) {
    internal_lexer_t *ilexer = (internal_lexer_t*) lexer;
    token_t token;

    // current char is 0 => EOF
    if (!ilexer->current_char) {
        token.type = TOKEN_EOF;
        goto lexer_get_next_token_end;
    }

    // skip white spaces
    while (is_white(ilexer->current_char) || ilexer->current_char == '\n' || ilexer->current_char == ';') {
        // current chat is ';' => comment till next line
        if (ilexer->current_char == ';') {
            while (ilexer->current_char != '\n' && ilexer->current_char) {
                ilexer->current_char = ilexer->str[++ilexer->index];
            }
        }

        next(ilexer);
    }

    char *str = ilexer->str + ilexer->index;

    // current char is 0 => EOF
    if (!ilexer->current_char) {
        token.type |= TOKEN_EOF;
        goto lexer_get_next_token_end;
    }

    // mnemonic
    if (*str != '#' && *str != '$' && *str != '(') {
        // count chars in token
        unsigned long char_count = 0;            
        for (;!is_delim(*str); ++char_count, ++str);

        if (char_count != 3) {
            goto lexer_get_next_token_error;
        }

        token.data.op_index = hash(str - 3);
        if (token.data.op_index) {
            token.type = TOKEN_OP;
            --token.data.op_index;
            goto lexer_get_next_token_end;
        }

        goto lexer_get_next_token_error;
    }

    // token is a directive
    if (*str == '.') {
        //TODO: implement
        goto lexer_get_next_token_error;
    }

    // token is a number
    token.type = TOKEN_NUMBER;

    if (is_abs(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_ABS;
        str += 4;
        goto lexer_get_next_token_end;
    }

    if (is_abx(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_ABX;
        str += 6;
        goto lexer_get_next_token_end;
    }

    if (is_aby(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_ABY;
        str += 6;
        goto lexer_get_next_token_end;
    }

    if (is_imm(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_IMM;
        str += 3;
        goto lexer_get_next_token_end;
    }

    if (is_ind(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_IND;
        str += 6;
        goto lexer_get_next_token_end;
    }

    if (is_inx(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_INX;
        str += 6;
        goto lexer_get_next_token_end;
    }

    if (is_iny(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_INY;
        str += 6;
        goto lexer_get_next_token_end;
    }

    if (is_rel_zpg(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_REL | LEGAL_MASK_ZPG;
        str += 2;
        goto lexer_get_next_token_end;
    }

    if (is_zpx(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_ZPX;
        str += 4;
        goto lexer_get_next_token_end;
    }

    if (is_zpy(str, &token.data.number_attrib.value)) {
        token.data.number_attrib.possible_addressing_modes = LEGAL_MASK_ZPY;
        str += 4;
        goto lexer_get_next_token_end;
    }

lexer_get_next_token_error:
    token.type |= TOKEN_INVALID;

lexer_get_next_token_end:
    if (ilexer->current_char) {
        ilexer->index = (unsigned long) (str - ilexer->str);
        ilexer->current_char = ilexer->str[++ilexer->index];
    }

    return token;
}