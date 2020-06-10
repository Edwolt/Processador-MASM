#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TAM_MEM 65536

typedef struct _Parser Parser;
typedef struct _Command Command;

enum CommandWhat {
    COMMAND_LABEL,
    COMMAND_CODE,
    COMMAND_CONST,
    COMMAND_NEXTLINE,
    COMMAND_ENDFILE
};

struct _Command {
    enum CommandWhat what;
    uint16_t command;
    char* label;
    uint16_t nextLine;
};

/**
 * Create a new parser for the file on path
 * Uses dynamic memory allocation
 */
Parser* createParser(char* path);

/**
 * Destruct parser and unalloc the memory
 */
void deleteParser(Parser* parser);

/**
 * Destruct command and unalloc the memory
 */
void deleteCommand(Command* command);

/**
 * Read the next chunck of code and returns what instruction it is
 */
Command* parser_nextChunck(Parser* parser);