#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct _Command Command;

/**
 * Tells what command struct store
 */
enum CommandType {
    COMMAND_NOTHING,  // Command has nothing                                                  { }
    COMMAND_LABEL,    // Command has a label l                                                {label: l}
    COMMAND_VALUE,    // Command has a const value v                                          {val: v}
    COMMAND_LIST,     // Command has a array a of length l of const value                     {len: l, list: a}
    COMMAND_SPACE,    // Command create n contiguos spaces with the value v                   {len: n; val: v}
    COMMAND_END,      // Command has nothing and tells that has no more command to be parsed  { }
};

/**
 * if the command is of type list, the length of array is on value
 */
struct _Command {
    enum CommandType type;  // Type of command
    char* label;
    uint16_t len;
    uint16_t val;
    uint16_t* list;
};

Command* commandNothing();                           // Creates a command of nothing type
Command* commandLabel(char* label);                  // Creates a command of label type
Command* commandValue(uint16_t val);               // Creates a command of value type
Command* commandList(uint16_t len, uint16_t* list);  // Creates a command of list type
Command* commandSpace(uint16_t val, uint16_t len);   // Creates a command of space type
Command* commandEnd();                               // Creates a command of type end

/**
 * Destruct command and unalloc the memory
 */
void deleteCommand(Command* command);