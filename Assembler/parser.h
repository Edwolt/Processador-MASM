#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define TAM_MEM 65536

typedef struct _Parser Parser;
typedef struct _Command Command;

enum Opcode {
    OP_J = 0xb0000,
    OP_CMP = 0xb0001,
    OP_STORE = 0xb0010,
    OP_LOAD = 0xb0011,
    OP_INOUT = 0xb0100,
    OP_SET = 0xb0101,
    OP_MOVE = 0xb0110,
    OP_ADD = 0xb0111,
    OP_SUB = 0xb1000,
    OP_MUL = 0xb1001,
    OP_DIV = 0xb1010,
    OP_SHIFT = 0xb1011,
    OP_AND = 0xb1100,
    OP_OR = 0xb1101,
    OP_XOR = 0xb1110,
    OP_NOT = 0xb1111,
    OP_NOTHING = -1,
    OP_LABEL = -2
};

enum CommandWhat {
    COMMAND_LABEL,
    COMMAND_CODE,
    COMMAND_NEXTLINE,
    COMMAND_ENDFILE
};

struct _Command {
    enum CommandWhat what;
    uint16_t command;
    char* label;
    uint16_t nextLine;

    // enum Opcode opcode;
    // union {
    //     char* label;
    //     struct {
    //         rz, rx, ry
    //     } reg;
    //     int nextLine;
    // };
};

/**
 * Create a new parser for the file on path
 * Uses dynamic memory allocation
 */
Parser* createParser(char* path);

/**
 * Destruct a parser and unalloc the memory
 */
void deleteParser(Parser* parser);

/**
 * Read the next chunck of code and returns what instruction it is
 */
Command* parser_nextChunck(Parser* parser);