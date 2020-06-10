#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_MEM 65536

typedef struct _Parser Parser;
typedef struct _Command Command;

enum CommandWhat {
    COMMAND_NOTHING,
    COMMAND_LABEL,
    COMMAND_CODE,
    COMMAND_CONST,
    COMMAND_NEXTLINE,
    COMMAND_ENDFILE,
};

struct _Command {
    enum CommandWhat what;
    uint16_t value;
    char* label;
    uint16_t nextLine;
};

/**
 * Create a new parser for the file on path
 * Uses dynamic memory allocation
 * 
 * Returns NULL if it isn't possible to alloc memory for parser
 * Returns NULL if it isn't possible to open the file
 */
Parser* createParser(char* path);

/**
 * Destruct parser and unalloc the memory
 */
void deleteParser(Parser* parser);

/**
 * Create Command struct
 * Uses dynamic memory allocation
 * 
 * Returns NULL if it isn't possible to alloc memory
 */
Command* createCommand();

/**
 * Destruct command and unalloc the memory
 */
void deleteCommand(Command* command);

/**
 * Read the next chunck of code and 
 * returns a Command struct with data to assemble the chunck
 */
Command* parseNext(Parser* parser);

#endif