#include "parser.h"

#define TAM_COMMAND 100

#define parserScan(parser, ...)          \
    fscanf((parser)->file, __VA_ARGS__); \
    if (feof((parser)->file)) return parserClose((parser))

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

//* =========================== *//
//* ===== Other Functions ===== *//
//* =========================== *//

static Command* parseConst(Parser* parser) {
    enum EvalFlag flag;
    bufferPushZ(parser);
    uint16_t num = evalConst(&flag, parser->buffer, parser->n);

    switch (flag) {
        case EVAL_OK:
            return commandValue(num);

        case EVAL_LARGE:
            printf(LINE_ERROR "Number is too large\n", parser->line);
            return commandNothing();

        case EVAL_ERROR:
            return commandNothing();

        case EVAL_INVALID:
            printf(LINE_ERROR "Invalid Const: %s\n", parser->line, parser->buffer);
            return commandNothing();

        default:
            printf(MARK_ASERROR "parserConst()\n");
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

        case EVAL_ERROR:
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

start:
    if (parser->state == START) {
        parserScan(parser, "%c", &c);
        while (isSpace(c) || isEnter(c)) {
            parserScan(parser, "%c", &c);
            if (isEnter(c)) parser->line++;
        }

        if (c == ';') {
            parserScan(parser, "%*[^\n]");
            goto start;
        } else if (isConstStart(c)) {
            parser->state = CONST;
        } else if (c == '\'') {
            parserScan(parser, "%c", &c);
            if (c == '\'') {
                printf(LINE_ERROR "Empty char", parser->line);
                return commandNothing();
            } else if (c == '\\') {
                parserScan(parser, "%c", &c);
                return parseEscapedChar(parser, c);

            } else {
            }
        } else if (c == '"') {
            parser->state = STRING;
        } else if (c == '[') {
            // TODO resolve array
        } else if (c == ']') {
            printf(LINE_ERROR "Unexpected token \"]\"\n", parser->line);
        } else if (c == ':') {
            printf(LINE_ERROR "Empty label\n", parser->line);
        } else {
            bufferPush(parser, c);
            parser->state = CODE;
        }
    }

    if (parser->state == CONST) {
        // TODO resolve const
        parserScan(parser, "%c", &c);
        while (isConst(parser->buffer, c)) {
            parserScan(parser, "%c", &c);
            bufferPush(parser, c);
        }
        if (isSpace(c)) {
            return parseConst(parser);
        } else if (isEnter(c)) {
            parser->line++;
            return parseConst(parser);
        } else if (c == '\'') {
            // TODO resolve char
        } else if (c == '"') {
            Command* command = parseConst(parser);
            parser->state = STRING;
            return command;
        } else if (c == '[') {
            // TODO resolve array
        } else if (c == ']') {
            printf(LINE_ERROR "Invalid Token \"]\"", parser->line);
        } else if (c == ':') {
            bufferPush(parser, c);
            goto label;
        } else {
            bufferPush(parser, c);
            parser->state = CODE;
            goto start;
        }

        return parseConst(parser);
    } else if (parser->state == STRING) {
        parserScan(parser, "%c", &c);
        if (isEnter(c)) {
            printf(LINE_ERROR "String isn't closed\n", parser->line);
        } else if (c == '\\') {
            parserScan(parser, "%c", &c);
            return parseEscapedChar(parser, c);
        } else if (c == '"') {
            parser->state = START;
            goto start;
        } else {
            return commandValue(c);
        }
    } else if (parser->state == CODE) {
        parserScan(parser, "%c", &c);
        while (!isSpace(c) && !isEnter(c)) {
            if (c == ':') {
                bufferPushZ(parser);
                return commandLabel(bufferReturn(parser));
            }
            parserScan(parser, "%c", &c);
        }

        if (isEnter(c)) {
            parser->line++;
            bufferPushZ(parser);
            printf(LINE_ERROR "Invalid instruction \"%s\"\n", parser->line, parser->buffer);
        }

        // TODO Resolve code
    }

    if (parser->state == LABEL) {
    label:
        return NULL;
        goto label;
    }

    printf(MARK_ASERROR "parseNext()");
    return NULL;
}

// Command* parseNext(Parser* parser) {
//     char c;
//     while (true) {
//         if (debug) {
//             printf(MARK_DEBUG "state %d | ", parser->state);
//             printf("buffer %-5d | ", parser->n);
//             for (int i = 0; i < parser->n; i++) printf("%c", parser->buffer[i]);
//             printf("\n");
//         }

//         if (parser->state == LABEL) {
//             parser->state = START;
//             return commandLabel(bufferReturn(parser));
//         }

//         fscanf(parser->file, "%c", &c);
//         if (feof(parser->file)) {
//             Command* command = parseEOF(parser);
//             parser->state = ENDFILE;
//             return command;
//         }

//         switch (parser->state) {
//             case START:
//                 if (isEnter(c)) {
//                     parser->line++;
//                     // parser->state = START;
//                 } else if (isSpace(c)) {
//                     // parser->state = START;
//                 } else if (c == ';') {
//                     parser->state = COMMENT;
//                 } else if (isConstStart(c)) {
//                     bufferPush(parser, c);
//                     parser->state = CONST;
//                 } else if (c == '\'') {
//                     parser->state = CHAR;
//                 } else if (c == '"') {
//                     parser->state = STRING;
//                 } else if (c == '[') {
//                     parser->state = ARRAY;
//                 } else if (c == ']') {
//                     parsingError(parser, "Unexpected token \"]\"");
//                 } else if (c == ':') {
//                     parsingError(parser, "Empty label");
//                     // parser->state = START;
//                 } else {  // Common char
//                     bufferPush(parser, c);
//                     parser->state = CODE;
//                 }
//                 break;

//             case COMMENT:
//                 fscanf(parser->file, "%*[^\n]");
//                 parser->state = START;

//                 // else parser->state = COMMENT;
//                 break;

//             case CONST:
//                 if (isEnter(c)) {
//                     Command* command = parseConst(parser);
//                     parser->line++;
//                     parser->state = START;
//                     return command;
//                 } else if (isSpace(c)) {
//                     Command* command = parseConst(parser);
//                     parser->state = START;
//                     return command;
//                 } else if (c == ';') {
//                     Command* command = parseConst(parser);
//                     parser->state = COMMENT;
//                     return command;
//                 } else if (c == '"') {
//                     Command* command = parseConst(parser);
//                     parser->state = STRING;
//                     return command;
//                 } else if (c == '[') {
//                     Command* command = parseConst(parser);
//                     parser->state = ARRAY;
//                     return command;
//                 } else if (c == ']') {
//                     Command* command = parseConst(parser);
//                     parsingError(parser, "Unexpected Token \"]\"");
//                     return command;
//                 } else if (c == ':') {
//                     parser->state = LABEL;
//                 } else if (isConst(parser, c)) {
//                     bufferPush(parser, c);
//                 } else {
//                     parser->state = CODE;
//                 }
//                 break;

//             case CHAR:  // TODO
//                 break;

//             case STRING:
//                 if (isEnter(c)) {
//                     parsingError(parser, "String isn't closed");
//                     parser->line++;
//                     parser->state = START;
//                     return commandValue(0);
//                 } else if (c == '\\') {
//                     fscanf(parser->file, "%c", &c);
//                     if (feof(parser->file)) {
//                         parsingError(parser, "String isn't closed");
//                         Command* command = parseEOF(parser);
//                         parser->state = ENDFILE;
//                         return command;
//                     }

//                     return parseEscapedChar(parser, c);
//                 } else if (c == '"') {
//                     parser->state = START;
//                     return commandValue((uint16_t)'\0');
//                 } else {
//                     // parser->state = STRING;
//                     return commandValue((uint16_t)c);
//                 }
//             case ARRAY:
//                 // TODO
//                 break;

//             case CODE:  // TODO
//                 if (isEnter(c)) {
//                     parsingWarning(parser, "Invalid instruction");
//                     parser->line++;
//                     bufferClear(parser);
//                     parser->state = START;
//                     return commandNothing();
//                 } else if (isSpace(c)) {
//                     // TODO Create Command
//                     // parser->buffer[parser->n] = '\0';
//                     // int rx, ry, rz, num;
//                     // if (strcmp(parser->buffer, "store")) {
//                     // fscanf()
//                     // }
//                     parser->n = 0;  // Clear buffer
//                 } else if (c == ':') {
//                     parser->state = LABEL;
//                 } else {
//                     bufferPush(parser, c);
//                 }
//                 break;

//             default:
//                 printf(MARK_ASERROR "parserNext(); state: %d\n", parser->state);
//                 parser->n = 0;  // Clear buffer
//                 parser->state = ENDFILE;
//                 return commandEnd();
//         }
//     }
//     printf(MARK_ASERROR "parserNext(); while break\n");
//     parser->n = 0;  // Clear buffer
//     return commandEnd();
// }