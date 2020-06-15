#include "parser.h"

#define TAM_COMMAND 100

/**
 * A enum with possible states of parser
 */
enum State {
    START,        // Prepared to read a new chunck
    COMMENT,      // Reading a comment
    DECIMAL,      // Reading a decimal value
    POSITIVE,     // Reading a positive decimal value
    NEGATIVE,     // Reading a negative decimal value
    BINARY,       // Reading a binary value
    HEXADECIMAL,  // Reading a hexadecimal value
    STRING,       // Reading a string
    LABEL,        // Readed a label
    CODE,         // Reading a instruction
    ENDFILE,      // The file is ended
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
    char* buffer;
    int n;
};

//* =========================== *//
//* ===== Private Methods ===== *//
//* =========================== *//

/**
 * Prints a error with str menssage
 */
static void parsingError(Parser* parser, const char* str) { printf("Line %d: %s\n", parser->line, str); }

/**
 * Returns if the character is a white space, but not a line break
 */
static bool isSpace(char c) { return c == '\t' || c == '\v' || c == ' '; }

/**
 * Returns if the character is a line break
 */
static bool isEnter(char c) { return c == '\n' || c == '\f' || c == '\r'; }

/**
 * Returns if the character is a decimal digit
 */
static bool isDecimal(char c) { return '0' <= c && c <= '9'; }

/**
 * Returns if the character is 0 or 1
 */
static bool isBinary(char c) { return c == '0' || c == '1'; }

/**
 * Returns if the character is a hexadecimal digit
 */
static bool isHexadecimal(char c) { return isDecimal(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }

/**
 * Return the value of the hexdecimal digit represented by character c
 */
static uint16_t hexToInt(char c) {
    if (isDecimal(c)) {
        return c - '0';
    } else if ('a' <= c && c <= 'f') {
        return (c - 'a' + 10);
    } else {  // if ('A' <= c && c <= 'F')
        return (c - 'A' + 10);
    }
}

/**
 * Return a command using decimal value on parser
 * Clear the parser buffer
 * Buffer: Until five decimal diigits (DDDDD)
 */
static Command* createDecimal(Parser* parser) {
    uint32_t num = 0;
    parser->buffer[parser->n] = '\0';

    if (parser->n > 5) {
        parsingError(parser, "Number is too large");
        parser->n = 0;  // clear buffer
        return commandNothing();
    }

    num = atoi(parser->buffer);
    parser->n = 0;  // clear buffer

    // Assert that num doesn't oveflow uint16_t
    if (num >= (1 << 16)) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    return commandValue((uint16_t)num);
}

/**
 * Return a command using the positive decimal value on buffer
 * Clear the parser buffer
 * Buffer: The '+' and until five decimal diigits (+DDDDD)
 */
static Command* createPositive(Parser* parser) {
    uint32_t num = 0;
    parser->buffer[0] = ' ';  // Remove sign on string
    parser->buffer[parser->n] = '\0';

    if (parser->n > 6) {
        parsingError(parser, "Number is too large");
        parser->n = 0;  // clear buffer
        return commandNothing();
    }

    num = atoi(parser->buffer);
    parser->n = 0;  // clear buffer

    if (num >= (1 << 15)) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    return commandValue((uint16_t)num);
}

/**
 * Return a command using the negative decimal value on buffer
 * Buffer: The '-' and until five decimal diigits (-DDDDD)
 */
static Command* createNegative(Parser* parser) {
    uint32_t num = 0;
    parser->buffer[0] = ' ';  // Remove sign on string
    parser->buffer[parser->n] = '\0';

    if (parser->n > 6) {
        parsingError(parser, "Number is too large");
        parser->n = 0;  // clear buffer
        return commandNothing();
    }

    num = atoi(parser->buffer);
    parser->n = 0;  // clear buffer

    if (num >= (1 << 15)) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }
    num = ~num + 1; // Two's complement

    return commandValue((uint16_t)num);
}

/**
 * Return a command using binary value on buffer
 * Buffer: The 'b' character and until 16 binary diigits (bBBBBBBBBBBBBBBBB)
 */
static Command* createBinary(Parser* parser) {
    int i;
    uint16_t num = 0;
    parser->buffer[parser->n] = '\0';

    if (parser->buffer[0] != 'b') {
        printf("Assembler fail on line %d: %s isn't binary\n", parser->n, parser->buffer);
        parser->n = 0;  // clear buffer
        return NULL;
    }

    if (parser->n > 17) {
        parsingError(parser, "Number is too large");
        parser->n = 0;  // clear buffer
        return commandNothing();
    }

    for (i = 1; i < parser->n; i++) {
        num <<= 1;
        if (parser->buffer[i] == '1') num |= 1;
    }

    parser->n = 0;  // clear buffer
    return commandValue(num);
}

/**
 * Return a command using binary value on buffer
 * Buffer: The 'x' character and until 4 hexadecimal diigits (xHHHH)
 */
static Command* createHexadecimal(Parser* parser) {
    int i;
    uint16_t num = 0;
    parser->buffer[parser->n] = '\0';

    if (parser->buffer[0] != 'x' && parser->buffer[0] != '#') {
        printf("Assembler fail on line %d: %s isn't hexadecimal\n", parser->line, parser->buffer);
        parser->n = 0;  // clear buffer
        return NULL;
    }

    if (parser->n > 5) {
        parsingError(parser, "Number is too large");
        parser->n = 0;  // clear buffer
        return commandNothing();
    }

    for (i = 1; i < parser->n; i++) {
        num <<= 4;
        num |= hexToInt(parser->buffer[i]);
    }

    parser->n = 0;  // clear buffer
    return commandValue(num);
}

/**
 * Return the value on parser if its a valid value
 * the parser state is important to this function
 */
static Command* createEOF(Parser* parser) {
    switch (parser->state) {
        case START:
            return commandEnd();

        case COMMENT:
            return commandEnd();

        case DECIMAL:
            return createDecimal(parser);

        case POSITIVE:
            return createPositive(parser);

        case NEGATIVE:
            return createNegative(parser);

        case BINARY:
            return createBinary(parser);

        case HEXADECIMAL:
            return createHexadecimal(parser);

        case STRING:
            parsingError(parser, "String isn't closed");
            return commandEnd();

        case CODE:
            parsingError(parser, "Command isn't supported");
            return commandEnd();

        default:
            printf("commandEOF() doesn't work\n");
            return commandEnd();
    }
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

    parser->buffer = malloc(TAM_COMMAND * sizeof(char));
    if (!parser->buffer) {
        free(parser);
        return NULL;
    }
    parser->n = 0;

    parser->state = START;
    parser->line = 1;

    return parser;
}

void deleteParser(Parser* parser) {
    if (!parser) return;
    if (parser->file) fclose(parser->file);
    if (parser->buffer) free(parser->buffer);
    free(parser);
}

Command* parseNext(Parser* parser) {
    if (!parser) return NULL;
    if (parser->state == ENDFILE) return commandEnd();
    if (!parser->buffer) {
        parser->buffer = malloc(TAM_COMMAND * sizeof(char));
        if (!parser->buffer) return NULL;  // Failed to allocate memomry for buffer
        parser->n = 0;
    };

    char c;
    while (true) {
        // printf("state: %d   ", parser->state);
        // printf("value(%d): ", parser->n);
        // for (int i = 0; i < parser->n; i++) printf("%c", parser->buffer[i]);
        // printf("\n");

        if (parser->state == LABEL) {
            parser->state = START;
            char* str = parser->buffer;
            parser->buffer = NULL;
            return commandLabel(str);
        }

        fscanf(parser->file, "%c", &c);
        if (feof(parser->file)) {
            Command* command = createEOF(parser);
            parser->state = ENDFILE;
            return command;
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
                } else if (isDecimal(c)) {
                    parser->buffer[parser->n++] = c;
                    parser->state = DECIMAL;
                } else if (c == '+') {
                    parser->buffer[parser->n++] = c;
                    parser->state = POSITIVE;
                } else if (c == '-') {
                    parser->buffer[parser->n++] = c;
                    parser->state = NEGATIVE;
                } else if (c == 'b') {
                    parser->buffer[parser->n++] = c;
                    parser->state = BINARY;
                } else if (c == 'x' || c == '#') {
                    parser->buffer[parser->n++] = c;
                    parser->state = HEXADECIMAL;
                } else if (c == '"') {
                    parser->buffer[parser->n++] = c;
                    parser->state = STRING;
                } else if (c == ':') {
                    parsingError(parser, "Empty label");
                    // parser->state = START;
                } else {  // Common char
                    parser->buffer[parser->n++] = c;
                    parser->state = CODE;
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
                if (isEnter(c)) {
                    Command* command = createDecimal(parser);
                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    Command* command = createDecimal(parser);
                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    Command* command = createDecimal(parser);
                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    Command* command = createDecimal(parser);
                    parser->state = STRING;
                    return command;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else if (isDecimal(c)) {
                    parser->buffer[parser->n++] = c;
                } else {  // Common char
                    parser->state = CODE;
                }
                break;

            case POSITIVE:
                if (isEnter(c)) {
                    Command* command = createPositive(parser);
                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    Command* command = createPositive(parser);
                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    Command* command = createPositive(parser);
                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    Command* command = createPositive(parser);
                    parser->state = STRING;
                    return command;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else if (isDecimal(c)) {
                    parser->buffer[parser->n++] = c;
                } else {  // Common char
                    parser->state = CODE;
                }
                break;

            case NEGATIVE:
                if (isEnter(c)) {
                    Command* command = createNegative(parser);
                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    Command* command = createNegative(parser);
                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    Command* command = createNegative(parser);
                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    Command* command = createNegative(parser);
                    parser->state = STRING;
                    return command;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else if (isDecimal(c)) {
                    parser->buffer[parser->n++] = c;
                } else {  // Common char
                    parser->state = CODE;
                }
                break;

            case BINARY:
                if (isEnter(c)) {
                    Command* command = createBinary(parser);
                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    Command* command = createBinary(parser);
                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    Command* command = createBinary(parser);
                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    Command* command = createBinary(parser);
                    parser->state = STRING;
                    return command;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else if (isBinary(c)) {
                    parser->buffer[parser->n++] = c;
                    // parser->state = BINARY;
                } else {  // Common char
                    parser->state = CODE;
                }
                break;

            case HEXADECIMAL:
                if (isEnter(c)) {
                    Command* command = createHexadecimal(parser);
                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    Command* command = createHexadecimal(parser);
                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    Command* command = createHexadecimal(parser);
                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    Command* command = createHexadecimal(parser);
                    parser->state = STRING;
                    return command;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else if (isHexadecimal(c)) {
                    parser->buffer[parser->n++] = c;
                    parser->state = HEXADECIMAL;
                } else {  // Common char
                    parser->state = CODE;
                }
                break;

            case STRING:
                if (isEnter(c)) {
                    parsingError(parser, "String isn't closed");
                    parser->line++;
                    parser->n = 0;  // clear buffer
                    parser->state = START;
                    return commandValue(0);
                } else if (c == '\\') {
                    fscanf(parser->file, "%c", &c);
                    if (feof(parser->file)) {
                        parsingError(parser, "String isn't closed");
                        Command* command = createEOF(parser);
                        parser->state = ENDFILE;
                        return command;
                    }

                    parser->n = 0;  // clear buffer
                    switch (c) {
                        case '0':
                            return commandValue('\0');
                        case 'a':
                            return commandValue('\a');
                        case 'b':
                            return commandValue('\b');
                        case 't':
                            return commandValue('\t');
                        case 'n':
                            return commandValue('\n');
                        case 'v':
                            return commandValue('\v');
                        case 'f':
                            return commandValue('\f');
                        case 'r':
                            return commandValue('\r');
                        case 'e':
                            return commandValue(27);
                        case '\'':
                            return commandValue('\'');
                        case '\"':
                            return commandValue('\"');
                        default:
                            parsingError(parser, "Invalid Character");
                            return commandValue(c);
                    }
                } else if (c == '"') {
                    parser->n = 0;  // Clear buffer
                    parser->state = START;
                    return commandValue((uint16_t)'\0');
                } else {
                    parser->n = 0;  // Clear buffer
                    // parser->state = STRING;
                    return commandValue((uint16_t)c);
                }

            case CODE:  // TODO
                if (isSpace(c)) {
                    // TODO Create Command
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {
                    parser->buffer[parser->n++] = c;
                }
                break;

            default:
                printf("parserNext() doesn't work; state = %d\n", parser->state);
                parser->n = 0;  // Clear buffer
                parser->state = ENDFILE;
                return commandEnd();
        }
    }
    printf("parserNext() The assembler doesn't work; while break\n");
    parser->n = 0;  // Clear buffer
    return commandEnd();
}