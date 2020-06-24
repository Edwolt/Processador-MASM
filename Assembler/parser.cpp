#include "parser.hpp"

inline static bool isSpace(char c) { return c == '\t' || c == '\v' || c == ' '; }
inline static bool isEnter(char c) { return c == '\n' || c == '\f' || c == '\r'; }

enum TokenType {
    DECIMAL,
    POSITIVE,
    NEGATIVE,
    BINARY,
    OCTAL,
    HEXADECIMAL,
    CHAR,
    ARRAY,
    STRING,
    LABEL,
    CODE,
    NOTHING,
};

struct Parser {
    ifstream file;
    int line = 0;
    map<string, vector<int>> labelsRef;
    map<string, int> labelsVal;
    vector<uint16_t> memory;

    Parser(string path) {
        file = ifstream(path);
    }

    string readChar() {
        string token;
        char c;
        file.get(c);
        if (file.eof()) return token;
        if (c != '\'') {
            file.unget();
            cbug << "readChar(): can't read a char from file" << endl;
            return token;
        }
        token.push_back(c);

        while (file.get(c)) {
            if (c == '\'' && token.back() != '\\') {
                token.push_back(c);
                break;
            } else if (c == ';') {
                file.unget();
                lerror(line) << "Char without close" << endl;
                token.clear();
                return token;
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "Char without close" << endl;
                token.clear();
                return token;
            }

            token.push_back(c);
        }

        return token;
    }

    string readString() {
        string token;
        char c;
        file.get(c);
        if (file.eof()) return token;
        if (c != '"') {
            file.unget();
            cbug << "getString(): can't read a string from file" << endl;
            return token;
        }
        token.push_back(c);

        while (file.get(c)) {
            if (c == '"' && token.back() != '\\') {
                token.push_back(c);
                break;
            } else if (c == ';') {
                file.unget();
                lerror(line) << "String without close" << endl;
                token.clear();
                return token;
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "String without close" << endl;
                token.clear();
                return token;
            }

            token.push_back(c);
        }

        return token;
    }

    string readArray() {
        string token;
        char c;
        file.get(c);
        if (file.eof()) return token;
        if (c != '[') {
            file.unget();
            cbug << "getArray(): can't read a array from file" << endl;
            return token;
        }
        token.push_back(c);

        while (file.get(c)) {
            if (c == ']') {
                token.push_back(c);
                break;
            } else if (c == ';') {
                file.unget();
                lerror(line) << "Array without close" << endl;
                token.clear();
                return token;
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "Array without close" << endl;
                token.clear();
                return token;
            }

            token.push_back(c);
        }

        return token;
    }

    string getToken() {
        string token;
        char c;
        while (file.get(c)) {
            if (isSpace(c)) {
            } else if (isEnter(c)) {
                line++;
            } else if (c == ';') {
                readComment();
            } else if (c == '"') {
                file.unget();
                return readString();
            } else if (c == '[') {
                file.unget();
                return readArray();
            } else if (c == ']') {
                lerror(line) << "Array without open, ignoring it" << endl;
            } else if (c == '\'') {
                file.unget();
                return readChar();
            } else {
                file.unget();
                break;
            }
        }

        if (file.eof()) return token;

        while (file.get(c)) {
            if (isSpace(c)) {
                break;
            } else if (isEnter(c)) {
                line++;
                break;
            } else if (c == ';' || c == '\'' || c == '"' || c == '[' || (c == '#' && !token.empty())) {
                file.unget();
                return token;
            } else if (c == ':') {
                token.push_back(c);
                return token;
            } else {
                token.push_back(c);
            }
        }
        return token;
    }

    TokenType categorizeToken(string token) {
        if (token.empty()) {
            return NOTHING;
        } else if (token.back() == ':') {
            return LABEL;
        } else if (token.front() == '"') {
            return STRING;
        } else if (token.front() == '\'') {
            return CHAR;
        } else if (token.front() == '[') {
            return ARRAY;
        } else if (token.front() == '+') {
            return POSITIVE;
        } else if (token.front() == '-') {
            return NEGATIVE;
        } else if (token.front() == '#') {
            return HEXADECIMAL;
        } else if (token.front() == 'b' && isBin(token)) {
            return BINARY;
        } else if (token.front() == 'o' && isOct(token)) {
            return OCTAL;
        } else if (token.front() == 'x' && isHex(token)) {
            return HEXADECIMAL;
        } else if (isDec(token)) {
            return DECIMAL;
        }
        return CODE;
    }

    void parseAll() {
        while (!file.eof()) {
            string token = getToken();
            switch (categorizeToken(token)) {
                case DECIMAL:
                    memory.push_back(evalDec(line, token));
                    cdebug << "Token: DEC\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << endl;
                    break;

                case POSITIVE:
                    memory.push_back(evalPos(line, token));
                    cdebug << "Token: POS\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << endl;
                    break;

                case NEGATIVE:
                    memory.push_back(evalNeg(line, token));
                    cdebug << "Token: NEG\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << endl;
                    break;

                case BINARY:
                    memory.push_back(evalBin(line, token));
                    cdebug << "Token: BIN\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << endl;
                    break;

                case OCTAL:
                    memory.push_back(evalOct(line, token));
                    cdebug << "Token: OCT\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << endl;
                    break;

                case HEXADECIMAL:
                    memory.push_back(evalHex(line, token));
                    cdebug << "Token: HEX\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << endl;
                    break;

                case CHAR:
                    memory.push_back(evalChar(line, token));
                    cdebug << "Token: CHAR\t | `" << token << '`' << endl;
                    cdebugr << memory.back() << " (" << (char)memory.back() << ')' << endl;
                    break;

                case ARRAY:
                    // type = string("ARR");
                    break;

                case STRING:
                    // type = string("STR");
                    break;

                case LABEL:
                    // type = string("LABEL");
                    break;

                case CODE:
                    // type = string("CODE");
                    break;

                case NOTHING:
                    // type = string("NOT");
                    break;

                default:
                    // type = string("DEF");
                    break;
            }
        }
    }

    void readComment() {
        char c;
        while (file.get(c)) {
            if (isEnter(c)) break;
        }

        line++;
    }

    vector<uint16_t> getMemory() { return memory; }
};

vector<uint16_t> parseCode(string path) {
    Parser parser(path);
    parser.parseAll();
    // parser.labels();
    return parser.getMemory();
}