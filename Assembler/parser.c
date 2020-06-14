#include "parser.h"

#define TAM_COMMAND 100

enum State {
    START,
    COMMENT,
    DECIMAL,
    POSITIVE_DECIMAL,
    NEGATIVE_DECIMAL,
    BINARY,
    HEXADECIMAL,
    STRING,
    LABEL,
    COMMAND,
    ENDFILE
};

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
};

/*
The parser is a finite state machine that parse the assembly code
*/
struct _Parser {
    FILE* file;
    enum State state;
    int line;
};

//* =========================== *//
//* ===== Private Methods ===== *//
//* =========================== *//

// TODO This function can be turned to a Macro
static void parsingError(Parser* parser, const char* str) {
    printf("Line %d: %s\n", parser->line, str);
}

/**
 * Returns if the character is a white space, but not a line break
 */
static bool isSpace(char c) {
    return c == '\t' ||
           c == '\v' ||
           c == ' ';
}

/**
 * Returns if the character is a line break
 */
static bool isEnter(char c) {
    return c == '\n' ||
           c == '\f' ||
           c == '\r';
}

/**
 * Returns if the character is a decimal digit
 */
static bool isDecimal(char c) {
    return 0 <= c - '0' && c - '0' < 10;
}

/**
 * Returns if the character is 0 or 1
 */
static bool isBinary(char c) {
    return c == '0' || c == '1';
}

/**
 * Returns if the character is a hexadecimal digit
 */
static bool isHexadecimal(char c) {
    return isDecimal(c) ||
           c == 'a' || c == 'A' ||
           c == 'b' || c == 'B' ||
           c == 'c' || c == 'C' ||
           c == 'd' || c == 'D' ||
           c == 'e' || c == 'E' ||
           c == 'f' || c == 'F';
}

static uint16_t hexToInt(char c) {
    if (isDecimal(c)) {
        return c - '0';
    } else if (c == 'a' || c == 'A') {
        return 10;
    } else if (c == 'b' || c == 'B') {
        return 11;
    } else if (c == 'c' || c == 'C') {
        return 12;
    } else if (c == 'd' || c == 'D') {
        return 13;
    } else if (c == 'e' || c == 'E') {
        return 14;
    } else {  // c == 'f' || c == 'F'
        return 15;
    }
}

/**
 * Transform a command on DECIMAL, POSITIVE_DECIMAL and NEGATIVE_DECIMAL state into 
 */
static Command* commandDecimal(Parser* parser, char* str) {
    uint32_t num = 0;

    // len > 6 doesn't overflow uint32_t (999999 == 0xF423F)
    if (strlen(str) > 6) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    num = atoi(str);
    free(str);

    // Assert that num doesn't oveflow uint16_t
    if (num >= (1 << 16)) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    // If state is signed decimal, assert that num is positive
    if ((parser->state == POSITIVE_DECIMAL || parser->state == NEGATIVE_DECIMAL) &&
        ((num & (1 << 15)) != 0)) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    // If state is negative decimal, tranform num using two's complement
    if (parser->state == NEGATIVE_DECIMAL) num = ~num + 1;

    return commandConst((uint16_t)num);
}

static Command* commandBinary(Parser* parser, char* str) {
    int i;
    uint16_t num = 0;
    const uint16_t bit = 1;

    int len = strlen(str);
    if (len > 16) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    for (i = 0; i < len; i++) {
        num <<= 1;
        if (str[i] == '1') num |= bit;
    }

    free(str);
    return commandConst(num);
}

static Command* commandHexadecimal(Parser* parser, char* str) {
    int i;
    uint16_t num = 0;

    int len = strlen(str);
    if (len > 4) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    for (i = 0; i < len; i++) {
        num <<= 4;
        num |= hexToInt(str[i]);
    }

    free(str);
    return commandConst(num);
}

//* ========================== *//
//* ===== Public Methods ===== *//
//* ========================== *//

Parser* createParser(char* path) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;

    parser->file = fopen(path, "r");
    if (!parser->file) {
        free(parser);
        return NULL;
    }

    parser->state = START;
    parser->line = 1;

    return parser;
}

void deleteParser(Parser* parser) {
    if (!parser) return;
    fclose(parser->file);
    free(parser);
}

Command* parseNext(Parser* parser) {
    if (!parser) return NULL;
    if (parser->state == ENDFILE) return commandEndFile();

    // TODO
    char* chunck = malloc(TAM_COMMAND * sizeof(char));
    if (!chunck) return NULL;

    int i = 0;
    char c;
    while (true) {
        // printf("state: %d   ", parser->state);
        // printf("value: %s\n", command->label);

        if (parser->state == LABEL) {
            chunck[i] = '\0';

            parser->state = START;
            return commandLabel(chunck);
        }

        fscanf(parser->file, "%c", &c);
        if (feof(parser->file)) {
            switch (parser->state) {
                case START:
                case COMMENT:
                    free(chunck);
                    parser->state = ENDFILE;
                    return commandEndFile();

                case DECIMAL:
                case POSITIVE_DECIMAL:
                case NEGATIVE_DECIMAL:
                    chunck[i++] = '\0';
                    parser->state = ENDFILE;
                    return commandDecimal(parser, chunck);

                case BINARY:
                    chunck[i++] = '\0';
                    parser->state = ENDFILE;
                    return commandBinary(parser, chunck);

                case HEXADECIMAL:
                    chunck[i++] = '\0';
                    parser->state = ENDFILE;
                    return commandHexadecimal(parser, chunck);

                case STRING:
                    parsingError(parser, "String isn't closed");
                    free(chunck);
                    parser->state = ENDFILE;
                    return commandEndFile();

                case COMMAND:
                    parsingError(parser, "Command isn't supported");
                    free(chunck);
                    parser->state = ENDFILE;
                    return commandEndFile();

                default:
                    printf("parseNext() doesn't work\n");
                    free(chunck);
                    parser->state = ENDFILE;
                    return commandEndFile();
            }
        }

        switch (parser->state) {
            case START:
                if (isEnter(c)) {
                    parser->line++;
                    // parser->state = START;
                } else if (isSpace(c)) {
                    // parser->state = START;
                } else if (c == ';') {
                    parser->state = COMMENT;
                } else if (c == '+') {
                    parser->state = POSITIVE_DECIMAL;
                } else if (c == '-') {
                    parser->state = NEGATIVE_DECIMAL;
                } else if (c == 'b') {
                    parser->state = BINARY;
                } else if (c == 'x') {
                    parser->state = HEXADECIMAL;
                } else if (c == '"') {
                    parser->state = STRING;
                } else if (c == ':') {
                    parsingError(parser, "Empty label");
                    // parser->state = START;
                } else if (isDecimal(c)) {
                    chunck[i++] = c;
                    parser->state = DECIMAL;
                } else {  // Common char
                    chunck[i++] = c;
                    parser->state = COMMAND;
                }
                break;

            case COMMENT:
                if (isEnter(c)) {
                    parser->line++;
                    parser->state = START;
                }
                // else parser->state = COMMENT;
                break;

            case DECIMAL:
            case POSITIVE_DECIMAL:
            case NEGATIVE_DECIMAL:
                if (isEnter(c)) {
                    chunck[i++] = '\0';
                    Command* command = commandDecimal(parser, chunck);

                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    chunck[i++] = '\0';
                    Command* command = commandDecimal(parser, chunck);

                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    chunck[i++] = '\0';
                    Command* command = commandDecimal(parser, chunck);

                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    chunck[i++] = '\0';
                    Command* command = commandDecimal(parser, chunck);

                    parser->state = STRING;
                    return command;
                } else if (isDecimal(c)) {
                    chunck[i++] = c;
                    // if(paser->state == DECIMAL) parser->state = DECIMAL;
                    // if(paser->state == POSITIVE->DECIMAL) parser->state = POSITIVE_DECIMAL;
                    // if(paser->state == NEGATIVE->DECIMAL) parser->state = NEGATIVE_DECIMAL;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {  // Common char
                    parser->state = COMMAND;
                }
                break;

            case BINARY:
                if (isEnter(c)) {
                    chunck[i++] = '\0';
                    Command* command = commandBinary(parser, chunck);

                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    chunck[i++] = '\0';
                    Command* command = commandBinary(parser, chunck);

                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    chunck[i++] = '\0';
                    Command* command = commandBinary(parser, chunck);

                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    chunck[i++] = '\0';
                    Command* command = commandBinary(parser, chunck);

                    parser->state = STRING;
                    return command;
                } else if (isBinary(c)) {
                    chunck[i++] = c;
                    // parser->state = BINARY;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {  // Common char
                    parser->state = COMMAND;
                }
                break;

            case HEXADECIMAL:
                if (isEnter(c)) {
                    chunck[i++] = '\0';
                    Command* command = commandHexadecimal(parser, chunck);

                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    chunck[i++] = '\0';
                    Command* command = commandHexadecimal(parser, chunck);

                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    chunck[i++] = '\0';
                    Command* command = commandHexadecimal(parser, chunck);

                    parser->state = COMMENT;
                    return command;
                } else if (isHexadecimal(c)) {
                    chunck[i++] = c;
                    parser->state = HEXADECIMAL;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {  // Common char
                    parser->state = COMMAND;
                }
                break;

            case STRING:
                if (isEnter(c)) {
                    parsingError(parser, "String isn't closed");
                    parser->line++;
                    free(chunck);

                    parser->state = START;
                    return commandConst(0);
                } else if (c == '\\') {
                    fscanf(parser->file, "%c", &c);
                    if (feof(parser->file)) {
                        parsingError(parser, "String isn't closed");
                        parser->state = ENDFILE;
                        return commandEndFile();
                    }

                    free(chunck);
                    switch (c) {
                        case '0':
                            return commandConst('\0');
                        case 'a':
                            return commandConst('\a');
                        case 'b':
                            return commandConst('\b');
                        case 't':
                            return commandConst('\t');
                        case 'n':
                            return commandConst('\n');
                        case 'v':
                            return commandConst('\v');
                        case 'f':
                            return commandConst('\f');
                        case 'r':
                            return commandConst('\r');
                        case 'e':
                            return commandConst(27);
                        case '\'':
                            return commandConst('\'');
                        case '\"':
                            return commandConst('\"');
                        default:
                            parsingError(parser, "Invalid Character");
                            return commandConst(c);
                    }
                } else if (c == '"') {
                    free(chunck);
                    parser->state = START;
                    return commandConst((uint16_t)'\0');
                } else {
                    free(chunck);
                    // parser->state = STRING;
                    return commandConst((uint16_t)c);
                }

            case COMMAND:
                chunck[i++] = c;
                if (c == ':') {
                    parser->state = LABEL;
                }
                // TODO
                break;

            default:
                printf("(parserNext) The assembler doesn't work");
                free(chunck);
                parser->state = ENDFILE;
                return commandEndFile();
        }
    }
    printf("(parserNext) The assembler doesn't work");
    free(chunck);
    return commandEndFile();
}