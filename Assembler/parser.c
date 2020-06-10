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
    LABEL,
    COMMAND,
    END
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
static void commandDecimal(Parser* parser, Command* command) {
    uint32_t num = 0;

    if (parser->state == DECIMAL) {
        if (strlen(command->label) > 5) {
            printf("Line %d: Number is too large\n", parser->line);
            goto error;
        }
        num = atoi(command->label);
        if (num > (uint32_t)0x1111111111111111) {
            printf("Line %d: Number is too large\n", parser->line);
            goto error;
        }

        command->what = COMMAND_CONST;
        command->value = (uint16_t)num;
    } else if (parser->state == POSITIVE_DECIMAL) {
        if (strlen(command->label) > 6) {
            printf("Line %d: Number is too large\n", parser->line);
            goto error;
        }

        num = atoi(command->label);
        if (num > (uint32_t)0x0111111111111111) {
            printf("Line %d: Number is too large\n", parser->line);
            goto error;
        }
        num = ~num + 1;  // Two's complement
    } else if (parser->state == NEGATIVE_DECIMAL) {
        if (strlen(command->label) > 6) {
            printf("Line %d: Number is too large\n", parser->line);
            goto error;
        }
        num = atoi(command->label);
        if (num > (uint32_t)0x0111111111111111) {
            printf("Line %d: Number is too large\n", parser->line);
            goto error;
        }

        command->what = COMMAND_CONST;
        command->value = (uint16_t)num;
    }

    command->what = COMMAND_CONST;
    command->value = (uint16_t)num;

    free(command->label);
    ;
    return;

error:
    command->value = 0;
    command->what = COMMAND_NOTHING;

    free(command->label);
    return;
}

static void commandBinary(Parser* parser, Command* command) {
    int i;
    uint16_t num = 0;
    uint16_t bit = 1;

    int len = strlen(command->label);
    if (len > 16) {
        printf("Line %d: Number is too large\n", parser->line);
        goto error;
    }

    for (i = 0; i < len; i++) {
        num <<= 1;
        if (command->label[i] == '1') num |= bit;
    }

    command->what = COMMAND_CONST;
    command->value = num;

    free(command->label);
    return;

error:
    command->value = 0;
    command->what = COMMAND_NOTHING;

    free(command->label);
    return;
}

static void commandHexadecimal(Parser* parser, Command* command) {
    int i;
    uint16_t num = 0;

    int len = strlen(command->label);
    if (len > 4) {
        printf("Line %d: Number is too large\n", parser->line);
        goto error;
    }

    for (i = 0; i < len; i++) {
        num <<= 4;
        num |= hexToInt(command->label[i]);
    }

    command->what = COMMAND_CONST;
    command->value = (uint16_t)num;

    free(command->label);
    return;

error:
    command->value = 0;
    command->what = COMMAND_NOTHING;

    free(command->label);
    return;
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

Command* createCommand() {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->what = COMMAND_NOTHING;
    command->value = 0;
    command->label = NULL;
    command->nextLine = 0;

    return command;
}

void deleteCommand(Command* command) {
    if (!command) return;
    if (command->what == COMMAND_LABEL && command->label) {
        free(command->label);
    }
    free(command);
}

Command* parseNext(Parser* parser) {
    if (!parser) return NULL;

    Command* command = createCommand();
    if (!command) return NULL;

    if (parser->state == END) {
        command->what = COMMAND_ENDFILE;
        return command;
    }

    command->label = malloc(TAM_COMMAND * sizeof(char));

    int i = 0;
    char c;
    while (true) {
        // printf("state: %d   ", parser->state);
        // printf("value: %s\n", command->label);

        if (parser->state == LABEL) {
            command->what = COMMAND_LABEL;
            command->label[i] = '\0';

            parser->state = START;
            return command;
        }

        fscanf(parser->file, "%c", &c);
        if (feof(parser->file)) {
            switch (parser->state) {
                case START:
                case COMMENT:
                    command->what = COMMAND_ENDFILE;
                    free(command->label);
                    break;

                case DECIMAL:
                case POSITIVE_DECIMAL:
                case NEGATIVE_DECIMAL:
                    command->label[i++] = '\0';
                    commandDecimal(parser, command);

                    parser->state = START;
                    parser->line++;
                    break;

                case BINARY:
                    command->label[i++] = '\0';
                    commandBinary(parser, command);

                    parser->state = START;
                    parser->line++;
                    break;

                case HEXADECIMAL:
                    command->label[i++] = '\0';
                    commandHexadecimal(parser, command);

                    parser->state = START;
                    parser->line++;
                    break;

                case COMMAND:
                    printf("Command isn't supported yet");
                    command->what = COMMAND_ENDFILE;
                    free(command->label);
                    break;

                default:
                    printf("(parser) The assembler doesn't work");
                    command->what = COMMAND_ENDFILE;
                    free(command->label);
                    parser->state = END;
            }
            parser->state = END;

            return command;
        }

        switch (parser->state) {
            case START:
                if (isEnter(c)) {
                    parser->line++;
                    parser->state = START;
                } else if (isSpace(c)) {
                    parser->state = START;
                } else if (c == ';') {
                    parser->state = COMMENT;
                } else if (c == 'x') {
                    parser->state = HEXADECIMAL;
                } else if (c == 'b') {
                    parser->state = BINARY;
                } else if (c == '+') {
                    parser->state = POSITIVE_DECIMAL;
                } else if (c == '-') {
                    parser->state = NEGATIVE_DECIMAL;
                } else if (c == ':') {
                    printf("Line %d: Empty label\n", parser->line);
                } else if (isDecimal(c)) {
                    command->label[i++] = c;
                    parser->state = DECIMAL;
                } else {  // Common char
                    command->label[i++] = c;
                    parser->state = COMMAND;
                }
                break;

            case COMMENT:
                if (isEnter(c)) {
                    parser->state = START;
                    parser->line++;
                }
                break;

            case DECIMAL:
            case POSITIVE_DECIMAL:
            case NEGATIVE_DECIMAL:
                if (isEnter(c)) {
                    command->label[i++] = '\0';
                    commandDecimal(parser, command);

                    parser->state = START;
                    parser->line++;

                    return command;
                } else if (isSpace(c)) {
                    command->label[i++] = '\0';
                    commandDecimal(parser, command);

                    parser->state = START;

                    return command;
                } else if (c == ';') {
                    command->label[i++] = '\0';
                    commandDecimal(parser, command);

                    parser->state = COMMENT;

                    return command;
                } else if (isDecimal(c)) {
                    command->label[i++] = c;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {  // Common char
                    parser->state = COMMAND;
                }
                break;

            case BINARY:
                if (isEnter(c)) {
                    command->label[i++] = '\0';
                    commandBinary(parser, command);

                    parser->state = START;
                    parser->line++;

                    return command;
                } else if (isSpace(c)) {
                    command->label[i++] = '\0';
                    commandBinary(parser, command);

                    parser->state = START;

                    return command;
                } else if (c == ';') {
                    command->label[i++] = '\0';
                    commandBinary(parser, command);

                    parser->state = COMMENT;

                    return command;
                } else if (isBinary(c)) {
                    command->label[i++] = c;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {  // Common char
                    parser->state = COMMAND;
                }
                break;

            case HEXADECIMAL:
                if (isEnter(c)) {
                    command->label[i++] = '\0';
                    commandHexadecimal(parser, command);

                    parser->state = START;
                    parser->line++;

                    return command;
                } else if (isSpace(c)) {
                    command->label[i++] = '\0';
                    commandHexadecimal(parser, command);

                    parser->state = START;

                    return command;
                } else if (c == ';') {
                    command->label[i++] = '\0';
                    commandHexadecimal(parser, command);

                    parser->state = COMMENT;

                    return command;
                } else if (isHexadecimal(c)) {
                    command->label[i++] = c;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {  // Common char
                    parser->state = COMMAND;
                }
                break;

            case COMMAND:
                command->label[i++] = c;
                if (c == ':') {
                    parser->state = LABEL;
                }
                // TODO
                break;

            default:
                printf("(parser) The assembler doesn't work");
                command->what = COMMAND_ENDFILE;
                free(command->label);
                parser->state = END;
        }
    }
    printf("(parser) The assembler doesn't work");
    free(command->label);
    command->what = COMMAND_ENDFILE;
    parser->state = END;
}