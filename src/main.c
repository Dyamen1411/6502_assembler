#include "lexer.h"
#include "opcodes.h"
#include "token.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_COUNT 64
#define ROM_SIZE 0x10000

typedef struct {
    char *input_file_name;
    char *output_file_name;
} program_data_t;

program_data_t* process_args(int argc, char **argv);
char *get_text_from_file(const char *file_name);
unsigned char get_tokens(char *file_content, token_t **tokens, unsigned long *token_count);
unsigned char parse_tokens_to_rom(token_t *tokens, unsigned long token_count, unsigned char *rom);
unsigned char write_rom_to_file(unsigned char *rom, char *output_file_name);
unsigned short mask_to_index(unsigned short mask);

int main(int argc, char **argv) {
    // Read the arguments passed
    program_data_t *program_data = process_args(argc, argv);
    if (program_data == NULL) {
        return 1;
    }

    // Read input file content
    char *file_content = get_text_from_file(program_data->input_file_name);
    if (file_content == NULL) {
        free(program_data->input_file_name);
        free(program_data->output_file_name);
        free(program_data);
        return 1;
    }

    // Break file content into tokens
    token_t *tokens;
    unsigned long token_count;
    if (get_tokens(file_content, &tokens, &token_count)) {
        free(program_data->input_file_name);
        free(program_data->output_file_name);
        free(program_data);
        free(file_content);
        free(tokens);
        return 1;
    }

    // No more need for the file content
    free(file_content);

    // Parse tokens to the rom buffer
    unsigned char *output_buffer = (unsigned char*) malloc(0x10000);
    if (parse_tokens_to_rom(tokens, token_count, output_buffer)) {
        free(program_data->input_file_name);
        free(program_data->output_file_name);
        free(program_data);
        free(output_buffer);
        free(tokens);
        return 1;
    }

    // No more need for the tokens
    free(tokens);
    
    // Write rom buffer to output file
    if (write_rom_to_file(output_buffer, program_data->output_file_name)) {
        free(program_data->input_file_name);
        free(program_data->output_file_name);
        free(program_data);
        free(output_buffer);
        return 1;
    }

    // Cleanup
    free(program_data->input_file_name);
    free(program_data->output_file_name);
    free(program_data);
    free(output_buffer);

    return 0;
}

// Creating program data from arguments passed.
program_data_t* process_args(int argc, char **argv) {
    if (argc < 2) {
        printf("Expected arguments.\nTry -h for help\n");
        return NULL;
    }

    unsigned char error = 0;
    program_data_t *program_data = (program_data_t*) malloc(sizeof(program_data_t));
    program_data->input_file_name = NULL;
    program_data->output_file_name = NULL;

    for (unsigned long i = 1; i < argc; ++i) {
        char *token = argv[i];

        if (*token == '-') {
            ++token;
            if (strcmp(token, "h") == 0) {
                printf("help\n");
                continue;
            }

            if (strcmp(token, "o") == 0) {
                if (program_data->output_file_name == NULL) {
                    if (++i < argc) {
                        token = argv[i];
                        unsigned long len = strlen(token);
                        program_data->output_file_name = (char*) malloc(len + 1);
                        memcpy(program_data->output_file_name, token, len);
                        program_data->output_file_name[len] = 0;
                        continue;
                    }

                    printf("Expected file name after -o.\n");
                    error = 1;
                    break;
                } else {
                    printf("Can't output to two different files.\n");
                    error = 1;
                    break;
                }
            }

            printf("Argument \"%s\" in unknown.\n", token + 1);
            error = 1;
            break;
        }

        if (program_data->input_file_name == NULL) {
            unsigned long len = strlen(token);
            program_data->input_file_name = (char*) malloc(len + 1);
            memcpy(program_data->input_file_name, token, len);
            program_data->input_file_name[len] = 0;
            continue;
        }

        printf("Can't process two files at one time!\n");
        error = 1;
        break;
    }

    if (!error && program_data->input_file_name == NULL) {
        printf("Expected input file!\n");
        error = 1;
    }

    if (!error && program_data->output_file_name == NULL) {
        unsigned long len = strlen(program_data->input_file_name);
        program_data->output_file_name = (char*) malloc(len + 4 + 1);
        memcpy(program_data->output_file_name, program_data->input_file_name, len);
        memcpy(program_data->output_file_name + len, ".bin", 4);
        program_data->output_file_name[len + 4] = 0;
    }

    if (!error && strcmp(program_data->input_file_name, program_data->output_file_name) == 0) {
        printf("Input and output can't be the same!\n");
        error = 1;
    }

    if (error) {
        if (program_data->input_file_name != NULL) {
            free(program_data->input_file_name);
        }
        if (program_data->output_file_name != NULL) {
            free(program_data->output_file_name);
        }
        free(program_data);
        return NULL;
    }

    return program_data;
}

// Reads input file content to a string 
char *get_text_from_file(const char *file_name) {
    FILE *in = fopen(file_name, "rb");
    if (in == NULL) {
        printf("Could not open file \"%s\"!\n", file_name);
        return NULL;
    }

    // Get file size
    fseek(in, 0, SEEK_END);
    unsigned long file_size = ftell(in);
    fseek(in, 0, SEEK_SET);

    // Reading file to $str
    char *str = (char*) malloc(file_size + 1);
    str[file_size] = 0;
    fread(str, 1, file_size, in);
    fclose(in);

    return str;
}

// Converts string to tokens
unsigned char get_tokens(char *file_content, token_t **tokens, unsigned long *token_count) {
    // Creating lexer
    lexer_t *lexer = lexer_create(file_content);
    *tokens = (token_t*) malloc(MAX_TOKEN_COUNT);
    *token_count = 1;

    token_t *token = *tokens;
    do {
        // Get next token and set id
        *token = lexer_get_next_token(lexer);
        if (token->type & TOKEN_INVALID) {
            printf("Token %d is invalid\n", *token_count - 1);
            lexer_delete(lexer);
            return 1;
        }

        token->id = *token_count;

        // No need to read more when EOF is hit
        if (token->type & TOKEN_EOF) {
            break;
        }

        // increment token ID
        ++*token_count;
        ++token;
    } while (*token_count < MAX_TOKEN_COUNT);

    lexer_delete(lexer);

    return 0;
}

unsigned short mask_to_index(unsigned short mask) {
#define convert(mode) if (mask & LEGAL_MASK_##mode) return INDEX_##mode;
    convert(ACC)
    convert(ABS)
    convert(ABX)
    convert(ABY)
    convert(IMM)
    convert(IMP)
    convert(IND)
    convert(INX)
    convert(INY)
    convert(REL)
    convert(ZPG)
    convert(ZPX)
    convert(ZPY)
    return AM_COUNT;
#undef convert
}

// Read and manipulate tokens to create the rom file
unsigned char parse_tokens_to_rom(token_t *tokens, unsigned long token_count, unsigned char *rom) {
    unsigned long rom_pointer = 0;

    for (unsigned long i = 0; i < token_count; ++i) {
        token_t token = tokens[i];

        // No need to process more when EOF token is read
        if (token.type & TOKEN_EOF) {
            break;
        }

        // Not enough space to write token. error
        if (rom_pointer + 1 >= ROM_SIZE) {
            printf("Code does not fit in rom.\n");
            return 1;
        }

        // Treated token should be an op
        if (!(token.type & TOKEN_OP)) {
            printf("not op %d\n", i);
            return 1;
        }

        // Getting data related to the opcode
        instruction_data_t opcode_data = instruction_data_table[token.data.op_index];

        // If IMP or ACC => write one byte
        if (opcode_data.legal_mask & LEGAL_MASK_IMP) {
            rom[rom_pointer++] = opcode_data.codes[INDEX_IMP];
            continue;
        }

        if ((opcode_data.legal_mask & LEGAL_MASK_ACC) 
            && (tokens[i + 1].type & (TOKEN_OP | TOKEN_EOF))) {
            rom[rom_pointer++] = opcode_data.codes[INDEX_ACC];
            continue;
        }

        // Not enough space to write token. error
        if (rom_pointer + 2 >= ROM_SIZE) {
            printf("Code does not fit in rom.\n");
            return 1;
        }

        // Next token expected.
        if (tokens[i + 1].type & TOKEN_EOF || !(tokens[i+1].type & TOKEN_NUMBER)) {
            printf("Missing operand.\n");
            return 1;
        }

        // Matching instrunction possible addressing modes with number.
        unsigned short common_addressing_modes = opcode_data.legal_mask & tokens[i+1].data.number_attrib.possible_addressing_modes;
        unsigned short index = mask_to_index(common_addressing_modes);

        // Instruction and number are not compatible. error
        if (index == AM_COUNT) {
            printf("Unknown opcode.\n");
            return 1;
        }

        ++i;

        // Writing instrunction and number (lo)
        rom[rom_pointer++] = opcode_data.codes[index];
        rom[rom_pointer++] = tokens[i].data.number_attrib.value & 0xFF;

        // If number is 16 bits, write number (hi)
        if (common_addressing_modes & (LEGAL_MASK_ABS | LEGAL_MASK_ABX | LEGAL_MASK_ABY | LEGAL_MASK_IND)) {
            // Not enough space to write token. error
            if (rom_pointer + 1 >= ROM_SIZE) {
                printf("Code does not fit un rom.\n");
                return 1;
            }
            
            rom[rom_pointer++] = (tokens[i].data.number_attrib.value >> 8) & 0xFF;
        }
    }

    return 0;
}

unsigned char write_rom_to_file(unsigned char *rom, char *output_file_name) {
    FILE *stream = fopen(output_file_name, "wb");
    if (stream == NULL) {
        printf("Could not open file.\n");
        return 1;
    }

    fwrite(rom, ROM_SIZE, 1, stream);
    fclose(stream);

    return 0;
}