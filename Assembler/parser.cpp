#include "parser.hpp"

template <typename T>
inline static void vecConcat(vector<T>& vec1, vector<T>& vec2) {
    vec1.reserve(vec1.size() + vec2.size());
    vec1.insert(vec1.end(), vec2.begin(), vec2.end());
}

enum TokenType {
    NUM,
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
            } else if (c == ';' || c == '\'' || c == '"' || c == '[' || c == ']' || (c == '#' && !token.empty())) {
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
        } else if (token.front() == '[') {
            return ARRAY;
        } else if (isNum(token)) {
            return NUM;
        } else if (isInvalidNum(token)) {
            lerror(line) << "Invalid number: `" << token << '`' << endl;
            return NOTHING;
        } else {
            return CODE;
        }
    }

    void parseAll() {
        while (!file.eof()) {
            pair<u16, u16> arr;
            string token = getToken();
            TokenType type = categorizeToken(token);

            if (type == NUM) {
                memory.push_back(evalNum(line, token));

                ldebug(line) << "Token: DEC\t | `" << token << "` ";
                cdebugr << memory.back() << endl;

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
                labelsVal[token] = memory.size();

                ldebug(line) << "Token: LAB\t | `" << token << '`';
                cdebugr << token << " = " << memory.size() << endl;

            } else if (type == CODE) {
                pair<u16, CodeType> code = createCode(line, token);
                u16 cval = code.first;
                CodeType ctype = code.second;
                ldebug(line) << "Token: CODE\t | `" << token << '`' << endl;

                if (ctype == NOT) {  // It's means thats is a label reference
                    labelsRef[token].push_back(memory.size());
                    memory.push_back(0);
                } else if (ctype == NOOP) {
                    memory.push_back(cval);
                } else if (ctype == RX) {
                    cval |= createRegister(line, getToken());
                    memory.push_back(cval);
                } else if (ctype == RY) {
                    cval |= createRegister(line, getToken());
                    cval |= createRegister(line, getToken()) << 4;
                    memory.push_back(cval);
                } else if (ctype == RZ) {
                    cval |= createRegister(line, getToken());
                    cval |= createRegister(line, getToken()) << 4;
                    cval |= createRegister(line, getToken()) << 8;
                    memory.push_back(cval);
                } else if (ctype == SET) {
                    cval |= createRegister(line, getToken());
                    memory.push_back(cval);

                    string arg = getToken();
                    u16 num;
                    if (isNum(arg)) {
                        num = evalNum(line, arg);
                    } else if (isInvalidNum(arg)) {  // TODO label
                        lerror(line) << "Invalid number: `" << token << "` (using 0)" << endl;
                        num = 0;
                    } else {
                        lerror(line) << "Expected a number, get `" << token << "` (using 0)" << endl;
                        num = 0;
                    }
                    memory.push_back(num);
                } else if (ctype == INOUT) {
                    // TODO
                } else if (ctype == IMM) {
                    cval |= createRegister(line, getToken());

                    string arg = getToken();
                    u16 num;
                    if (isNum(arg)) {
                        num = evalNumImm(line, arg);
                    } else if (isInvalidNum(arg)) {
                        lerror(line) << "Invalid number: `" << token << "` (using 0)" << endl;
                        num = 0;
                    } else {
                        lerror(line) << "Expected a number, get `" << token << "` (using 0)" << endl;
                        num = 0;
                    }
                    cval |= num << 4;

                    memory.push_back(cval);
                } else {
                    cbug << "parseAll() CODE: `" << token << '`' << endl;
                }

                cdebugr << hex << code.first << " with args " << cval << dec << " marked as " << ctype << endl;

            } else if (type == NOTHING) {
                ldebug(line) << "Token: NOT\t | `" << token << "` " << endl;

            } else {
                cbug << "The assembler isn't prepared to read this token: " << token << endl;
            }

            if (memory.size() > MEM_DEPTH) {
                memory.resize(MEM_DEPTH);
                cerror << "The program don't fit on the memory, assembler stoped on line " << line << endl;
                break;
            }
        }
    }

    void resolveLabels() {
        // Special labels
        if (labelsVal.find("@end") != labelsVal.end()) {
            cwarning << "You can't override the special label `@end`, that is reserved to save the last used position" << endl;
        }
        for (u16 i : labelsRef["@end"]) {
            memory[i] = memory.size();
            cdebug << "Memory[" << i << "] <- @end = " << memory.size() << endl;
        }
        labelsRef.erase("@end");

        if (labelsVal.find("@here") != labelsVal.end()) {
            cwarning << "You can't override the special label `@here`, that is reserved to refer to the position where it's put" << endl;
        }
        for (u16 i : labelsRef["@here"]) {
            memory[i] = i;
            cdebug << "Memory[" << i << "] <- @here = " << i << endl;
        }
        labelsRef.erase("@here");

        for (pair<string, vector<u16>> i : labelsRef) {
            if (labelsVal.find(i.first) != labelsVal.end()) {  // Label was declared
                int val = labelsVal[i.first];
                cdebug << "Label " << i.first << " is " << val << endl;
            } else {
                cerror << i.first << " was referred, but not declared" << endl;
            }
        }
    }
};

vector<u16> assembleCode(string path) {
    Parser parser(path);
    if (!parser.file.is_open()) {
        cerror << "Can't open the source file: " << path << endl;
        return vector<u16>(0);
    }

    parser.parseAll();
    cstep << "Parsed all tokens" << endl
          << endl;

    parser.resolveLabels();
    cstep << "Label finished" << endl
          << endl;
    return parser.memory;
}