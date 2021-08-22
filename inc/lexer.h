#ifndef LEXER_H
#define LEXER_H

#include "token.h"

// typedef struct {
//     char *str;
//     unsigned long str_len;
//     unsigned long index;
//     char current_char;
// } internal_lexer_t;

typedef struct internal_lexer_t lexer_t;

lexer_t* lexer_create(char *string);
token_t lexer_get_next_token(lexer_t *lexer);
void lexer_delete(lexer_t *lexer);

#endif