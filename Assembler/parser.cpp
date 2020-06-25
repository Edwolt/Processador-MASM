#include "parser.hpp"

template <typename T>
inline static void vecConcat(vector<T>& vec1, vector<T>& vec2) {
    vec1.reserve(vec1.size() + vec2.size());
    vec1.insert(vec1.end(), vec2.begin(), vec2.end());
}

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
    map<string, vector<u16>> labelsRef;
    map<string, u16> labelsVal;
    vector<u16> memory;

    Parser(string path) {
        file = ifstream(path);
    }
    ~Parser() {
        file.close();
    }

    //* ========================== *//
    //* ===== Read Functions ===== *//
    //* ========================== *//

   private:
    void readComment() {
        char c;
        while (file.get(c)) {
            if (isEnter(c)) break;
        }
        line++;
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
                lwarning(line) << "Char without close, closing for you" << endl;
                token.push_back('\'');
                return token;
            } else if (isEnter(c)) {
                line++;
                lwarning(line) << "Char without close, closing for you" << endl;
                token.push_back('\'');
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
                lwarning(line) << "String without close, closing it for you" << endl;
                token.push_back('"');
                return token;
            } else if (isEnter(c)) {
                line++;
                lwarning(line) << "String without close, closing it for you" << endl;
                token.push_back('"');
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
                lwarning(line) << "Array without close, closing it for you" << endl;
                token.push_back(']');
                return token;
            } else if (isEnter(c)) {
                line++;
                lwarning(line) << "Array without close, closing it for you" << endl;
                token.push_back(']');
                return token;
            }

            token.push_back(c);
        }

        return token;
    }

   public:
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
        } else if (isDec(token)) {
            return DECIMAL;
        } else if (isPos(token)) {
            return POSITIVE;
        } else if (token.front() == '+') {
            lerror(line) << "Invalid number: " << token << endl;
            return NOTHING;
        } else if (isNeg(token)) {
            return NEGATIVE;
        } else if (token.front() == '-') {
            lerror(line) << "Invalid number: " << token << endl;
            return NOTHING;
        } else if (isBin(token)) {
            return BINARY;
        } else if (isOct(token)) {
            return OCTAL;
        } else if (isHex(token)) {
            return HEXADECIMAL;
        } else if (token.front() == '#') {
            lerror(line) << "Invalid number: " << token << endl;
            return NOTHING;
        }
        return CODE;
    }

    void parseAll() {
        while (!file.eof()) {
            pair<u16, u16> arr;
            string token = getToken();
            TokenType type = categorizeToken(token);

            if (type == DECIMAL) {
                memory.push_back(evalDec(line, token));

                ldebug(line) << "Token: DEC\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

            } else if (type == POSITIVE) {
                memory.push_back(evalPos(line, token));

                ldebug(line) << "Token: POS\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

            } else if (type == NEGATIVE) {
                memory.push_back(evalNeg(line, token));

                ldebug(line) << "Token: NEG\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

            } else if (type == BINARY) {
                memory.push_back(evalBin(line, token));

                ldebug(line) << "Token: BIN\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

            } else if (type == OCTAL) {
                memory.push_back(evalOct(line, token));

                ldebug(line) << "Token: OCT\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

            } else if (type == HEXADECIMAL) {
                memory.push_back(evalHex(line, token));

                ldebug(line) << "Token: HEX\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

            } else if (type == CHAR) {
                memory.push_back(evalChar(line, token));

                ldebug(line) << "Token: CHAR\t | `" << token << "` ";
                cdebugr << memory.back() << " (" << (char)memory.back() << ')' << endl;

            } else if (type == ARRAY) {
                arr = evalArr(line, token);
                memory.resize(memory.size() + arr.first, arr.second);

                ldebug(line) << "Token: ARR\t | `" << token << "` ";
                cdebugr << '[' << arr.first << ',' << arr.second << ']' << endl;

            } else if (type == STRING) {
                vector<u16> vec = evalStr(line, token);
                vecConcat(memory, vec);

                ldebug(line) << "Token: ARR\t | `" << token << '`' << endl;

            } else if (type == LABEL) {
                token.pop_back();
                if (labelsVal.find(token) != labelsVal.end()) {  // Token found
                    lerror(line) << "Duplicated label: `" << token << "` Conidering the last declaration" << endl;
                }
                labelsVal.insert(pair<string, u16>(token, memory.size()));

                ldebug(line) << "Token: LAB\t | `" << token << "` ";
                cdebugr << token << " = " << memory.size() << endl;

            } else if (type == CODE) {
                // TODO

            } else if (type == NOTHING) {
                ldebug(line) << "Token: NOT\t | `" << token << "` " << endl;

            } else {
                cbug << "The assembler isn't prepared to read this token: " << token << endl;
            }

            if (memory.size() > MEM_DEPTH) {
                memory.resize(MEM_DEPTH);
                cerror << "The program is more large than memory, assembler stoped on line " << line << endl;
                break;
            }
        }
        cdebugr << "Parsed all tokens" << endl;
    }
};

vector<u16> assembleCode(string path) {
    Parser parser(path);
    if (!parser.file.is_open()) {
        cerror << "Can't open the source file: " << path << endl;
        return vector<u16>(0);
    }

    parser.parseAll();
    // parser.labels();
    return parser.memory;
}