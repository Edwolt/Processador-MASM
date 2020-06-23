#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "message.h"
#include "value.h"

#define TAM_MEM 65536

bool debug;

typedef struct _Parser Parser;

/**
 * Creates a new parser for the file on path
 * Uses dynamic memory allocation
 * 
 * Returns NULL if it can't to alloc memory for parser
 * Returns NULL if it can't to open the file
 */
Parser* createParser(char* path);

/**
 * Destructs parser and unalloc the memory
 */
void deleteParser(Parser* parser);

/**
 * Creates Command struct
 * Uses dynamic memory allocation
 * 
 * Returns NULL if it can't to alloc memory
 */
Command* createCommand();

/**
 * Destructs command and unalloc the memory
 */
void deleteCommand(Command* command);

/**
 * Reads the next chunck of code and 
 * returns a Command struct with data to assemble the chunck
 */
Command* parseNext(Parser* parser);