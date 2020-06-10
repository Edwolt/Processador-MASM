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

static bool isSpace(char c) {
    return c == '\t' ||
           c == '\v' ||
           c == ' ';
}

static bool isEnter(char c) {
    return c == '\n' ||
           c == '\f' ||
           c == '\r';
}

static bool isDecimal(char c) {
    return 0 <= c - '0' && c - '0' < 10;
}

static Command* createCommand() {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->what = COMMAND_CONST;
    command->command = 0;
    command->label = NULL;
    command->nextLine = 0;

    return command;
}

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

void deleteCommand(Command* command) {
    if (!command) return;
    if (command->what == COMMAND_LABEL && command->label) {
        free(command->label);
    }
    free(command);
}

Command* parser_nextChunck(Parser* parser) {
    if (!parser) return NULL;

    Command* command = createCommand();
    if (!command) return NULL;

    if (parser->state == END) {
        command->what = COMMAND_ENDFILE;
        return command;
    }

    command->label = malloc(TAM_COMMAND * sizeof(char));

    int i;
    char c;

    while (true) {
        if (parser->state == LABEL) {
            command->what = COMMAND_LABEL;
            parser->state = START;
            return command;
        }

        fscanf(parser->file, "%c", &c);
        if (feof(parser->file)) {
            command->what = COMMAND_ENDFILE;
            free(command->label);
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
                if (isEnter(c)) {
                    if (i > 5) {
                        printf("Line %d: This number is too large\n", parser->line);
                    }
                    uint32_t num = atoi(command->label);
                    if (num > UINT16_MAX) {
                        printf("Line %d: This number is too large\n", parser->line);
                    }
                    free(command->label);
                    command->label = NULL;
                    command->what = COMMAND_CONST;
                    command->command = (uint16_t)num;

                    parser->state = START;
                    parser->line++;

                    return command;
                } else if (isSpace(c)) {
                    if (i > 5) {
                        printf("Line %d: This number is too large\n", parser->line);
                    }
                    uint32_t num = atoi(command->label);
                    if (num > UINT16_MAX) {
                        printf("Line %d: This number is too large\n", parser->line);
                    }
                    free(command->label);
                    command->label = NULL;
                    command->what = COMMAND_CONST;
                    command->command = (uint16_t)num;

                    parser->state = START;

                    return command;
                } else if (c == ';') {
                    if (i > 5) {
                        printf("Line %d: This number is too large\n", parser->line);
                    }
                    uint32_t num = atoi(command->label);
                    if (num > UINT16_MAX) {
                        printf("Line %d: This number is too large\n", parser->line);
                    }
                    free(command->label);
                    command->label = NULL;
                    command->what = COMMAND_CONST;
                    command->command = (uint16_t)num;

                    parser->state = COMMENT;

                    return command;
                } else if (isDecimal(c)) {
                    command->label[i++] = c;
                } else if (c == ':') {
                    parser->state = LABEL;
                }
                break;

            case POSITIVE_DECIMAL:
                // TODO Looks like DECIMAL
                break;

            case NEGATIVE_DECIMAL:
                // TODO Looks like DECIMAL
                break;

            case BINARY:
                // TODO Looks like DECIMAL
                break;

            case HEXADECIMAL:
                // TODO Looks like DECIMAL
                break;

            case COMMAND:
                // TODO
                break;

            default:
                printf("The assembler doesn't work");
                free(command->label);
                command->what = COMMAND_ENDFILE;
                parser->state = END;
        }
    }
    printf("The assembler doesn't work");
    free(command->label);
    command->what = COMMAND_ENDFILE;
    parser->state = END;
}