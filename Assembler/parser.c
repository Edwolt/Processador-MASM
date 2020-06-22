#include "parser.h"

#include "message.h"

#define TAM_COMMAND 100

/**
 * A enum with possible states of parser
 */
enum State {
    START,    // Prepared to read a new chunck
    COMMENT,  // Reading a comment
    CONST,    // Reading a const
    STRING,   // Reading a string
    ARRAY,    // Reading a [const]
    LABEL,    // Readed a label
    CODE,     // Reading a instruction
    ENDFILE,  // The file is ended
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

//* ========================== *//
//* ===== Inline Methods ===== *//
//* ========================== *//

static inline void parsingError(Parser* parser, const char* str, ...) {
    printf(MARK_ERROR "\033[1m Line %d:\033[0m \n", parser->line);
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}
static inline void parsingWarning(Parser* parser, const char* str, ...) {
    printf(MARK_WARNING "\033[1m Line %d:\033[0m \n", parser->line);
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}

inline static bool isSpace(char c) { return c == '\t' || c == '\v' || c == ' '; }
inline static bool isEnter(char c) { return c == '\n' || c == '\f' || c == '\r'; }

inline static bool isDecimal(char c) { return '0' <= c && c <= '9'; }
inline static bool isBinary(char c) { return c == '0' || c == '1'; }
inline static bool isHexadecimal(char c) {
    return isDecimal(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}

inline static uint16_t hexToInt(char c) {
    if (isDecimal(c)) {
        return c - '0';
    } else if ('a' <= c && c <= 'f') {
        return (c - 'a' + 10);
    } else {  // if ('A' <= c && c <= 'F')
        return (c - 'A' + 10);
    }
}

inline static void bufferPush(Parser* parser, char c) { parser->buffer[parser->n++] = c; }
inline static void bufferPushZ(Parser* parser) { parser->buffer[parser->n] = '\0'; }
inline static void bufferClear(Parser* parser) { parser->n = 0; }
inline static char* bufferReturn(Parser* parser) {
    parser->buffer = NULL;
    char* aux;
    return aux;
}

//* =========================== *//
//* ===== Private Methods ===== *//
//* =========================== *//

/**
 * Return a command using decimal value on parser
 * Clear the parser buffer
 * Buffer: Until five decimal diigits (DDDDD)
 */
static Command* createDecimal(Parser* parser) {
    uint32_t num = 0;

    if (parser->n > 5) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    num = atoi(parser->buffer);

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

    if (parser->n > 6) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    num = atoi(parser->buffer);

    // Assert that num is positive and doesn't overflow uint_16
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

    if (parser->n > 6) {
        parsingError(parser, "Number is too large");
        parser->n = 0;  // clear buffer
        return commandNothing();
    }

    num = atoi(parser->buffer);

    // Assert that num is positive and doesn't overflow uint_16
    if (num >= (1 << 15)) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }
    num = ~num + 1;  // Turns number negative with two's complement

    return commandValue((uint16_t)num);
}

/**
 * Return a command using binary value on buffer
 * Buffer: The 'b' character and until 16 binary diigits (bBBBBBBBBBBBBBBBB)
 */
static Command* createBinary(Parser* parser) {
    int i;
    uint16_t num = 0;

    if (parser->buffer[0] != 'b') {
        printf("Assembler fail on line %d: %s isn't binary\n", parser->n, parser->buffer);
        return NULL;
    }

    if (parser->n > 17) {
        parsingError(parser, "Number is too large");
        return commandNothing();
    }

    for (i = 1; i < parser->n; i++) {
        num <<= 1;
        if (parser->buffer[i] == '1') num |= 1;
    }

    return commandValue(num);
}

/**
 * Return a command using binary value on buffer
 * Buffer: The 'x' or '#' character and until 4 hexadecimal diigits (xHHHH or #HHHH)
 */
static Command* createHexadecimal(Parser* parser) {
    int i;
    uint16_t num = 0;
    parser->buffer[parser->n] = '\0';

    if (parser->buffer[0] != 'x' && parser->buffer[0] != '#') {
        parsingError(parser, "%s isn't hexadecimal", parser->buffer);
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

static bool isConst(Parser* parser, char c) {
    bufferPushZ(parser);
    switch (parser->buffer[0]) {
        case '-':
        case '+':
            return isDecimal(c);
        case 'b':
            return isBinary(c);
        case 'x':
        case '#':
            return isHexadecimal(c);
        default:  // Decimal value
            return isDecimal(c);
    }
}

static Command* createConst(Parser* parser) {
    switch (parser->buffer[0]) {
        case '-':
            return createNegative(parser);
        case '+':
            return createPositive(parser);
        case 'b':
            return createBinary(parser);
        case 'x':
        case '#':
            return createHexadecimal(parser);
        default:  // Is Decimal
            return createDecimal(parser);
    }

    if (parser->buffer[0] == 'x') {
        return createHexadecimal(parser);
    }
}

/**
 * If the parser have a valid value, create a command with it
 * Else return a command end

 * The parser state is important to this function
 */
static Command* createEOF(Parser* parser) {
    switch (parser->state) {
        case START:
            return commandEnd();

        case COMMENT:
            return commandEnd();

        case CONST:
            return createConst(parser);

        case STRING:
            parsingError(parser, "String isn't closed");
            return commandEnd();

        case CODE:
            parsingError(parser, "Command isn't supported");
            return commandEnd();

        default:
            printf(MARK_ASERROR "commandEOF()\n");
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
        if (debug) {
            printf(MARK_DEBUG "state %d | ", parser->state);
            printf("buffer %-5d | ", parser->n);
            for (int i = 0; i < parser->n; i++) printf("%c", parser->buffer[i]);
            printf("\n");
        }

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
                    bufferPush(parser, c);
                    parser->state = CONST;
                } else if (c == '+') {
                    bufferPush(parser, c);
                    parser->state = CONST;
                } else if (c == '-') {
                    bufferPush(parser, c);
                    parser->state = CONST;
                } else if (c == 'b') {
                    bufferPush(parser, c);
                    parser->state = CONST;
                } else if (c == 'x' || c == '#') {
                    bufferPush(parser, c);
                    parser->state = CONST;
                } else if (c == '"') {
                    bufferPush(parser, c);
                    parser->state = STRING;
                } else if (c == ':') {
                    parsingError(parser, "Empty label");
                    // parser->state = START;
                } else {  // Common char
                    bufferPush(parser, c);
                    parser->state = CODE;
                }
                break;

            case COMMENT:
                fscanf(parser->file, "%*[\n]");
                parser->state = START;

                // else parser->state = COMMENT;
                break;

            case CONST:
                if (isEnter(c)) {
                    Command* command = createConst(parser);
                    parser->line++;
                    parser->state = START;
                    return command;
                } else if (isSpace(c)) {
                    Command* command = createConst(parser);
                    parser->state = START;
                    return command;
                } else if (c == ';') {
                    Command* command = createConst(parser);
                    parser->state = COMMENT;
                    return command;
                } else if (c == '"') {
                    Command* command = createConst(parser);
                    return command;
                } else if (c == ':') {
                    parser->state = LABEL;
                } else if (isConst(parser, c)) {
                    bufferPush(parser, c);
                } else {
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
                    bufferClear(parser);
                    parser->state = START;
                    return commandValue((uint16_t)'\0');
                } else {
                    bufferClear(parser);
                    // parser->state = STRING;
                    return commandValue((uint16_t)c);
                }

            case CODE:  // TODO
                if (isEnter(c)) {
                    parsingWarning(parser, "Invalid instruction");
                    parser->line++;
                    bufferClear(parser);
                    parser->state = START;
                    return commandNothing();
                } else if (isSpace(c)) {
                    // TODO Create Command
                    // parser->buffer[parser->n] = '\0';
                    // int rx, ry, rz, num;
                    // if (strcmp(parser->buffer, "store")) {
                    // fscanf()
                    // }
                    parser->n = 0;  // Clear buffer
                } else if (c == ':') {
                    parser->state = LABEL;
                } else {
                    bufferPush(parser, c);
                }
                break;

            default:
                printf(MARK_ASERROR "parserNext(); state: %d\n", parser->state);
                parser->n = 0;  // Clear buffer
                parser->state = ENDFILE;
                return commandEnd();
        }
    }
    printf(MARK_ASERROR "parserNext(); while break\n");
    parser->n = 0;  // Clear buffer
    return commandEnd();
}