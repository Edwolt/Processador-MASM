#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <stdlib.h>

typedef struct _Command Command;

enum CommandType {
    COMMAND_NOTHING,
    COMMAND_LABEL,
    COMMAND_CODE,
    COMMAND_CONST,
    COMMAND_NEXTLINE,
    COMMAND_ENDFILE,
};

struct _Command {
    enum CommandType type;
    uint16_t value;
    char* label;
    uint16_t nextLine;
};

/**
 * Creates a command of nothing type 
 */
Command* commandNothing();

/**
 * Creates a command of label type
 */
Command* commandLabel(char* label);

/**
 * Creates a command of value type
 */
Command* commandCode(uint16_t value);

/**
 * Creates a command of const type
 */
Command* commandConst(uint16_t value);

/**
 * Creates a command of next line type
 */
Command* commandNextLine(uint16_t value, uint16_t nextLine);

/**
 * Creates a command of type end file
 */
Command* commandEndFile();

/**
 * Destruct command and unalloc the memory
 */
void deleteCommand(Command* command);

#endif