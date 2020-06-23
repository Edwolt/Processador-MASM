#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "message.h"

/**
 * Flag with the result of a eval
 */
enum EvalFlag {
    EVAL_OK,       // Success evaluate
    EVAL_LARGE,    // Number is too large
    EVAL_INVALID,  // Received a string that don't match with pattern
    EVAL_BUG,    // Exist a bug on eval
};

/**
 * eval a string with decimal value:
 * pattern: DDDDD
 * example: 12345
 */
uint16_t evalDecimal(enum EvalFlag* flag, char* str, int n);

/**
 * eval a string with positive decimal value:
 * pattern: +DDDDD
 * example: +12345
 */
uint16_t evalPositive(enum EvalFlag* flag, char* str, int n);

/**
 * eval a string with a negative decimal:
 * pattern: -DDDDD
 * example: -12345
 */
uint16_t evalNegative(enum EvalFlag* flag, char* str, int n);

/**
 * eval a string with a binary number
 * pattern: bBBBBBBBBBBBBBBBB
 * example: b1010101010101010
 */
uint16_t evalBinary(enum EvalFlag* flag, char* str, int n);

/**
 * eval a string with a hexadecimal number
 * pattern: xHHHH or #HHHH
 * example: xABCD
 * example: #ABCD
 */
uint16_t evalHexadecimal(enum EvalFlag* flag, char* str, int n);

/**
 * Parse a string with:
 * * A decimal value
 * * A positive decimal value
 * * A negative decimal value
 * * A binary value
 * * A hexadecimal value
 */
uint16_t evalConst(enum EvalFlag* flag, char* str, int n);

/**
 * Return a command with the value of char c if it was escaped
 * example: if c is '0' returns '\0'
 */
uint16_t evalEscapedChar(enum EvalFlag* flag, char c);