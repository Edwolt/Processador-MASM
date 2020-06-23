#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "message.h"

enum EvalFlag {
    EVAL_OK, // Success evaluate
    EVAL_LARGE,  // Number is too large
    EVAL_INVALID,
    EVAL_ERROR,
};

/**
 * Parse the buffer to a decimal command
 * Buffer: Until five decimal diigits (DDDDD)
 */
uint16_t evalDecimal(enum EvalFlag* flag, char* str, int n);

/**
 * Parse the buffer to positive decimal command
 * Buffer: The '+' and until five decimal diigits (+DDDDD)
 */
uint16_t evalPositive(enum EvalFlag* flag, char* str, int n);

/**
 * Parse the buffer to a negative decimal command
 * Buffer: The '-' and until five decimal diigits (-DDDDD)
 */
uint16_t evalNegative(enum EvalFlag* flag, char* str, int n);

/**
 * Parse the buffer to a binary command
 * Buffer: The 'b' character and until 16 binary diigits (bBBBBBBBBBBBBBBBB)
 */
uint16_t evalBinary(enum EvalFlag* flag, char* str, int n);

/**
 * Parse the buffer to a hexadecimal command
 * Buffer: The 'x' or '#' character and until 4 hexadecimal diigits (xHHHH or #HHHH)
 */
uint16_t evalHexadecimal(enum EvalFlag* flag, char* str, int n);

/**
 * Parse the buffer to a const value, then creates a command with it
 * Clear the buffer
 * Buffer: The 'b' character and until 16 binary diigits (bBBBBBBBBBBBBBBBB)
 */
uint16_t evalConst(enum EvalFlag* flag, char* str, int n);

/**
 * Return a command with the value of char c
 * if it was escaped
 * example: if c == 0 returns a command with value '\0'
 */
uint16_t evalEscapedChar(enum EvalFlag* flag, char c);