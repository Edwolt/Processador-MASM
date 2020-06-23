#include "value.h"

#define NOTHING 0

inline static bool isDecimal(char c) { return '0' <= c && c <= '9'; }
inline static bool isBinary(char c) { return c == '0' || c == '1'; }
inline static bool isHexadecimal(char c) { return isDecimal(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
inline static bool isConstStart(char c) { return isDecimal(c) || c == '+' || c == '-' || c == 'b' || c == 'x' || c == '#'; }

inline static uint16_t hexToInt(char c) {
    if (isDecimal(c)) {
        return c - '0';
    } else if ('a' <= c && c <= 'f') {
        return (c - 'a' + 10);
    } else {  // if ('A' <= c && c <= 'F')
        return (c - 'A' + 10);
    }
}

uint16_t evalDecimal(enum EvalFlag* flag, char* str, int n) {
    uint32_t num;

    if (n > 5) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    num = atoi(str);

    // Assert that num doesn't oveflow uint16_t
    if (num >= (1 << 16)) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    *flag = EVAL_OK;
    return (uint16_t)num;
}

uint16_t evalPositive(enum EvalFlag* flag, char* str, int n) {
    uint32_t num;
    str[0] = ' ';  // Remove sign on string

    if (n > 6) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    num = atoi(str);

    // Assert that num is positive and doesn't overflow uint_16
    if (num >= (1 << 15)) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    *flag = EVAL_OK;
    return (uint16_t)num;
}

uint16_t evalNegative(enum EvalFlag* flag, char* str, int n) {
    uint32_t num;

    str[0] = ' ';  // Remove sign on string

    if (n > 6) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    num = atoi(str);

    // Assert that num is positive and doesn't overflow uint_16
    if (num >= (1 << 15)) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }
    num = ~num + 1;  // Turns number negative with two's complement

    *flag = EVAL_OK;
    return (uint16_t)num;
}

uint16_t evalBinary(enum EvalFlag* flag, char* str, int n) {
    int i;
    uint16_t num;

    if (n > 17) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    for (i = 1; i < n; i++) {
        num <<= 1;
        if (str[i] == '1') num |= 1;
    }

    *flag = EVAL_OK;
    return num;
}

uint16_t evalHexadecimal(enum EvalFlag* flag, char* str, int n) {
    int i;
    uint16_t num;

    if (n > 5) {
        *flag = EVAL_LARGE;
        return NOTHING;
    }

    for (i = 1; i < n; i++) {
        num <<= 4;
        num |= hexToInt(str[i]);
    }

    *flag = EVAL_OK;
    return num;
}

uint16_t evalConst(enum EvalFlag* flag, char* str, int n) {
    if (isDecimal(str[0])) {
        return evalDecimal(flag, str, n);
    } else if (str[0] == '-') {
        return evalNegative(flag, str, n);
    } else if (str[0] == '+') {
        return evalPositive(flag, str, n);
    } else if (str[0] == 'b') {
        return evalBinary(flag, str, n);
    } else if (str[0] == 'x' || str[0] == '#') {
        return evalHexadecimal(flag, str, n);
    } else {
        printf(MARK_BUG "evalConst(): \"%s\" isn't a CONST\n", str);
        *flag = EVAL_BUG;
        return NOTHING;
    }
}

uint16_t evalEscapedChar(enum EvalFlag* flag, char c) {
    switch (c) {
        case '0': return '\0';
        case 'a': return '\a';
        case 'b': return '\b';
        case 't': return '\t';
        case 'n': return '\n';
        case 'v': return '\v';
        case 'f': return '\f';
        case 'r': return '\r';
        case 'e': return 27;
        case '\'': return '\'';
        case '\"': return '\"';
        default:
            *flag = EVAL_INVALID;
            return NOTHING;
    }
}