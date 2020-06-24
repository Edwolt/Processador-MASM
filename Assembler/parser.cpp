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
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "String without close" << endl;
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
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "String without close" << endl;
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
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "Array without close" << endl;
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
        if (token.front() == '"') {
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
        } else if (token.front() == 'b') {
            return NOTHING;
        } else if (token.front() == 'o') {
            return NOTHING;
        } else if (token.front() == 'x') {
            return NOTHING;
        } else if (token.back() == ':') {
            return LABEL;
        } else {
            return NOTHING;
            // TODO Verify all char to determinates if its a code or a decimal
        }
    }

    void parseAll() {
        while (!file.eof()) {
            string token = getToken();
            string type;
            switch (categorizeToken(token)) {
                case DECIMAL: type = string("DECIMAL"); break;
                case POSITIVE: type = string("POSITIVE"); break;
                case NEGATIVE: type = string("NEGATIVE"); break;
                case BINARY: type = string("BINARY"); break;
                case OCTAL: type = string("OCTAL"); break;
                case HEXADECIMAL: type = string("HEXADECIMAL"); break;
                case CHAR: type = string("CHAR   "); break;
                case ARRAY: type = string("ARRAY   "); break;
                case STRING: type = string("STRING"); break;
                case LABEL: type = string("LABEL   "); break;
                case CODE: type = string("CODE   "); break;
                case NOTHING: type = string("NOTHING"); break;
                default: type = string("DEFAULT"); break;
            }
            cdebug << "Token: " << type << "\t`" << token << '`' << endl;
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