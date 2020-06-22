#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct _Command Command;

/**
 * Tells what command struct store
 */
enum CommandType {
    COMMAND_NOTHING,  // Command has nothing
    COMMAND_LABEL,    // Command has a label
    COMMAND_VALUE,    // Command has a const value
    COMMAND_LIST,     // Command has a array of const value
    COMMAND_SPACE,    // Command will create value space with nothing
    COMMAND_END,      // Command has nothing and has no more command to be parsed
};

/**
 * if the command is of type list, the length of array is on value
 */
struct _Command {
    enum CommandType type;  // Type of command
    char* label;
    uint16_t value;  // Const value or the length of array on list
    uint16_t* list;
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
Command* commandValue(uint16_t value);

/**
 * Creates a command of next line type
 */
Command* commandList(uint16_t len, uint16_t* list);
/**
 * Creates a command of type end
 */
Command* commandEnd();

/**
 * Destruct command and unalloc the memory
 */
void deleteCommand(Command* command);