#include "parser.hpp"

inline static bool isSpace(char c) { return c == '\t' || c == '\v' || c == ' '; }
inline static bool isEnter(char c) { return c == '\n' || c == '\f' || c == '\r'; }

struct Parser {
    ifstream file;
    int line = 0;
    map<string, vector<int>> labelsRef;
    map<string, int> labelsVal;
    vector<uint16_t> memory;

    Parser(string path) {
        file = ifstream(path);
    }

    string getString() {
        string str;
        char c;
        file.get(c);
        if (file.eof()) return str;
        if (c != '"') {
            file.unget();
            cbug << "getString(): can't read a string from file" << endl;
            return str;
        }
        str.push_back(c);

        while (file.get(c)) {
            if (c == '"') {
                str.push_back(c);
                break;
            } else if (isEnter(c)) {
                line++;
                lerror(line) << "String without close" << endl;
                return str;
            }

            str.push_back(c);
        }

        return str;
    }

    string getToken() {
        string token;
        char c;
        while (file.get(c)) {
            if (isSpace(c)) {
            } else if (isEnter(c)) {
                line++;
            } else if (c == ';') {
                commment();
            } else if (c == '"') {
                file.unget();
                return getString();
            } else {
                file.unget();
                break;
            }
        }

        if (file.eof()) return token;

        file.get(c);
        if (file.eof()) return token;
        token.push_back(c);

        while (file.get(c)) {
            if (isSpace(c)) {
                break;
            } else if (isEnter(c)) {
                line++;
                break;
            } else if (c == ';' || c == '\'' || c == '[' || c == '#') {
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

    void
    parseAll() {
        while (!file.eof()) {
            string token = getToken();
            cdebug << "Token: " << token << endl;
        }
    }

    void commment() {
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