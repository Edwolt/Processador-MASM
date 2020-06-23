#include "parser.h"

#define TAM_COMMAND 100

/**
 * Macro to fscanf to turn easy to read the file
 * Use the label parser_eof: to handle the case of the fscanf reach the end of the file
 */
#define parserScan(parser, ...) \
    if (fscanf((parser)->file, __VA_ARGS__) == EOF) goto parser_eof

/**
 * A enum with possible states of parser
 */
enum State {
    START,    // Prepared to read a new chunck
    COMMENT,  // Reading a comment
    CONST,    // Reading a const
    CHAR,     // Reading a char
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

//* ========================= *//
//* ===== Bool Funtions ===== *//
//* ========================= *//

inline static bool isSpace(char c) { return c == '\t' || c == '\v' || c == ' '; }
inline static bool isEnter(char c) { return c == '\n' || c == '\f' || c == '\r'; }

inline static bool isDecimal(char c) { return '0' <= c && c <= '9'; }
inline static bool isBinary(char c) { return c == '0' || c == '1'; }
inline static bool isHexadecimal(char c) { return isDecimal(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
inline static bool isConstStart(char c) { return isDecimal(c) || c == '+' || c == '-' || c == 'b' || c == 'x' || c == '#'; }

static bool isConst(char* str, char c) {
    if (isDecimal(c) || str[0] == '-' || str[0] == '+') {
        return isDecimal(c);
    } else if (str[0] == 'b') {
        return isBinary(c);
    } else if (str[0] == 'x' || str[0] == '#') {
        return isHexadecimal(c);
    } else {
        return false;
    }
}

//* ============================ *//
//* ===== Buffer Functions ===== *//
//* ============================ *//

inline static void bufferPush(Parser* parser, char c) { parser->buffer[parser->n++] = c; }
inline static void bufferPushZ(Parser* parser) { parser->buffer[parser->n] = '\0'; }
inline static void bufferClear(Parser* parser) { parser->n = 0; }
inline static char* bufferReturn(Parser* parser) {
    char* aux = parser->buffer;
    parser->buffer = NULL;
    return aux;
}

//* ============================= *//
//* ===== Private Functions ===== *//
//* ============================= *//

/**
 * Returns a command created with parser->buffer
 * then clear the buffer
 */
static Command* parseConst(Parser* parser) {
    enum EvalFlag flag;
    bufferPushZ(parser);
    uint16_t num = evalConst(&flag, parser->buffer, parser->n);
    bufferClear(parser);

    switch (flag) {
        case EVAL_OK:
            return commandValue(num);

        case EVAL_LARGE:
            printf(LINE_ERROR "Number is too large\n", parser->line);
            return commandNothing();

        case EVAL_BUG:
            return commandNothing();

        case EVAL_INVALID:
            printf(LINE_ERROR "Invalid Const: %s\n", parser->line, parser->buffer);
            return commandNothing();

        default:
            printf(MARK_BUG "parserConst()\n");
            return commandEnd();
    }
}

static Command* parseEscapedChar(Parser* parser, char c) {
    enum EvalFlag flag;
    uint16_t val = evalEscapedChar(&flag, c);

    switch (flag) {
        case EVAL_OK:
            return commandValue(val);

        case EVAL_LARGE:
            return commandNothing(LINE_ERROR "Number is too large\n", parser->line);

        case EVAL_BUG:
            return commandNothing();

        case EVAL_INVALID:
            printf(LINE_ERROR "Invalid Character: '\\%c'\n", parser->line, c);
            return commandNothing();

        default: return commandNothing();
    }
}

/**
 * If the parser have a valid value, create a command with it
 * Else return a command end
 * then close the parser->file and set parser state to END
 * 
 * The parser state is important
 */
static Command* parserClose(Parser* parser) {
    fclose(parser->file);
    parser->file = NULL;

    switch (parser->state) {
        case START: return commandEnd();
        case COMMENT: return commandEnd();
        case CONST: return parseConst(parser);

        case STRING:
            printf(LINE_ERROR "String isn't closed\n", parser->line);
            return commandEnd();

        case CODE:
            printf(LINE_ERROR "Command isn't supported", parser->line);
            return commandEnd();

        default:
            printf(MARK_BUG "commandEOF()\n");
            return commandEnd();
    }
}

//* =========================== *//
//* ===== State Functions ===== *//
//* =========================== *//

/**
 * If you readed a single quote you can call this meuhod to return a command with the value on char
 * This method reads until the other single quote 
 */
static Command* resolveChar(Parser* parser) {
    char c;

    parserScan(parser, "%c", &c);
    if (c == '\'') {
        printf(LINE_ERROR "Empty char\n", parser->line);
        return commandNothing();
    } else if (c == '\\') {
        parserScan(parser, "%c", &c);
        return parseEscapedChar(parser, c);
    } else {
        return commandValue(c);
    }

parser_eof:
    printf(LINE_ERROR "Empty or invalid char\n", parser->line);
    return commandNothing();
}

/**
 * Consumes space until reach a common char
 * Can chage the state to ENDFILE
 */
static void consumeSpace(Parser* parser) {
    char c;
    do {
        parserScan(parser, "%c", &c);
    } while (isSpace(c));

    ungetc(c, parser->file);
    return;

parser_eof:
    parser->state = ENDFILE;
}
/**
 * Consumes space and enters until reach a common char
 * Can chage the state to ENDFILE
 */
static void consumeWhiteSpace(Parser* parser) {
    char c;
    do {
        parserScan(parser, "%c", &c);
        if (isEnter(c)) parser->line++;
    } while (isSpace(c) || isEnter(c));

    ungetc(c, parser->file);
    return;

parser_eof:
    parser->state = ENDFILE;
}

/**
 * Reads the comment until reach the end of line
 * Can chage the state to ENDFILE
 */
inline static void consumeComment(Parser* parser) {
    parserScan(parser, "%*[^n]");
    return;

parser_eof:
    parser->state = ENDFILE;
}

/**
 * Read a const and save on buffer
 * Can change the state to LABEL, CODE or ENDFILE
 */
static void readConst(Parser* parser) {
    char c;
    do {
        bufferPush(parser, c);
        parserScan(parser, "%c", &c);
    } while (isConst(parser->buffer, c));
    ungetc(c, parser->file);
    return;

parser_eof:
    parser->state = ENDFILE;
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

    parser->buffer = NULL;
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

    // Create a parser buffer
    if (!parser->buffer) {
        parser->buffer = malloc(TAM_COMMAND * sizeof(char));
        if (!parser->buffer) {
            printf(MARK_BUG "The assembler can't run without alocate memory\n");
            return NULL;
        }
        parser->n = 0;
    };

    char c;
    while (true) {
        switch (parser->state) {
            case START:
                consumeWhiteSpace(parser);
                if (parser->state != START) break;

                parserScan(parser, "%c", &c);
                if (c == ';') {
                    parserScan(parser, "%*[^\n]");
                } else if (isConstStart(c)) {
                    readConst(parser);
                    parserScan(parser, "%c", &c);

                    if (isSpace(c) || isEnter(c) ||
                        c == '\'' || c == '"' || c == '[' || c == ']') {
                        ungetc(c, parser->file);
                        return parseConst(parser);
                    } else if (c == ':') {
                        bufferPush(parser, c);
                        parser->state = LABEL;
                    } else {
                        bufferPush(parser, c);
                        parser->state = CODE;
                    }
                } else if (c == '\'') {
                    return resolveChar(parser);
                } else if (c == '"') {
                    parser->state = STRING;
                } else if (c == '[') {
                    uint16_t len;
                    uint16_t val = 0;
                    enum EvalFlag flag;

                    consumeSpace(parser);
                    parserScan(parser, "%c", &c);
                    if (c == '\'') {
                        Command* aux = resolveChar(parser);
                        len = aux->val;
                        free(aux);
                    } else {
                        readConst(parser);
                        len = evalConst(&flag, parser->buffer, parser->n);
                    }

                    parserScan(parser, "%c", &c);
                    if (isSpace(c)) {
                        consumeSpace(parser);
                        parserScan(parser, "%c", &c);
                    }

                    if (c == ']') {
                        return commandSpace(0, len);
                    } else if (c == ',') {
                    } else if (isEnter(c)) {
                        printf(LINE_ERROR "Expected a number\n", parser->line++);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    } else if (c == '\'') {
                        printf(LINE_ERROR "Expected a \",\" or a number\n", parser->line);
                    } else if (c == '"' || c == '[' || c == ':') {
                        printf(LINE_ERROR "Expected a number\n", parser->line);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    } else {
                        printf(LINE_ERROR "Expected a number\n", parser->line);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    }

                    consumeSpace(parser);
                    parserScan(parser, "%c", &c);
                    if (c == '\'') {
                        Command* aux = resolveChar(parser);
                        val = aux->val;
                        free(aux);
                    } else {
                        readConst(parser);
                        len = evalConst(&flag, parser->buffer, parser->n);
                    }

                    parserScan(parser, "%c", &c);
                    if (isSpace(c)) {
                        consumeSpace(parser);
                        parserScan(parser, "%c", &c);
                    }

                    if (c == ']') {
                        return commandSpace(val, len);
                    } else if (c == ',') {
                        printf(LINE_ERROR "Expected a number\n", parser->line);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    } else if (isEnter(c)) {
                        printf(LINE_ERROR "Expected a number\n", parser->line++);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    } else if (c == '\'') {
                        printf(LINE_ERROR "Expected a number\n", parser->line);
                    } else if (c == '"' || c == '[' || c == ':') {
                        printf(LINE_ERROR "Expected a number\n", parser->line);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    } else {
                        printf(LINE_ERROR "Expected a number\n", parser->line);
                        parserScan(parser, "%*[^[]%*c");
                        break;
                    }
                    printf(LINE_ERROR "Erro estranho\n", parser->line);

                } else if (c == ']') {
                    printf(LINE_ERROR "Unexpected token \"]\"\n", parser->line);
                } else if (c == ':') {
                    printf(LINE_ERROR "Empty label\n", parser->line);
                } else {
                    bufferPush(parser, c);
                    parser->state = CODE;
                }
                break;

            case STRING:
                parserScan(parser, "%c", &c);
                if (isEnter(c)) {
                    printf(LINE_ERROR "String isn't closed\n", parser->line);
                    parser->line++;
                    parser->state = START;
                    return commandValue('\0');
                } else if (isSpace(c)) {
                    return commandValue(' ');
                } else if (c == '\\') {
                    parserScan(parser, "%c", &c);
                    return parseEscapedChar(parser, c);
                } else if (c == '"') {
                    parser->state = START;
                    return commandValue('\0');
                } else {
                    return commandValue((uint16_t)c);
                }

            case LABEL:
                printf(MARK_BUG "Label not implemented\n");
                return NULL;

            case CODE:
                printf(MARK_BUG "Label not implemented\n");
                return NULL;

                /*
            // case COMMENT:
            // case CONST:
            // case CHAR:
            // case ARRAY:
            case LABEL:
            case CODE:
                return;
                */

            case ENDFILE: return commandEnd();

            default:
                printf(MARK_DEBUG "%d\n", parser->state);
                return NULL;
        }
    }

parser_eof:
    return parserClose(parser);
}