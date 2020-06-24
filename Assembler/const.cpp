#include "const.hpp"

inline bool isDec(char c) { return '0' <= c && c <= '9'; }
inline bool isBin(char c) { return c == '0' || c == '1'; }
inline bool isOct(char c) { return '0' <= c && c <= '7'; }
inline bool isHex(char c) { return isDec(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }

bool isDec(string str) {
    for (char i : str) {
        if (!isDec(i)) return false;
    }

    return true;
}

bool isBin(string str) {
    if (str.front() != 'b') return false;

    for (unsigned i = 1; i < str.size(); i++) {
        if (!isBin(str[i])) return false;
    }

    return true;
}

bool isOct(string str) {
    if (str.front() != 'o') return false;

    for (unsigned i = 1; i < str.size(); i++) {
        if (!isOct(str[i])) return false;
    }

    return true;
}

bool isHex(string str) {
    if (str.front() != 'x' && str.front() != '#') return false;
    if (str.front() == '#') cbug << "Hexadecimal starting with '#' don't need to use the isHex function" << endl;

    for (unsigned i = 1; i < str.size(); i++) {
        if (!isHex(str[i])) return false;
    }

    return true;
}

inline static void pushBin(uint16_t& n, char c) {
    n <<= 1;
    if (c == '1') n |= 1;
}

inline static void pushOct(uint16_t& n, char c) {
    n <<= 3;
    n |= (c - '0');
}

inline static void pushHex(uint16_t& n, char c) {
    n <<= 4;
    if (isDec(c)) {
        n |= (c - '0');
    } else if ('a' <= c && c <= 'f') {
        n |= (c - 'a' + 10);
    } else {  // if ('A' <= c && c <= 'F')
        n |= (c - 'A' + 10);
    }
}

uint16_t evalDec(int line, string token) {
    uint32_t num = stoi(token);
    if (token.size() > 5 || num >= (1 << 16)) {
        uint16_t num = stoi(token);
        lerror(line) << "Number is too large (returning " << (uint16_t)num << ')' << endl;
    }
    return num;
}

uint16_t evalPos(int line, string token) {
    token.erase(0, 1);
    uint32_t num = stoi(token);
    if (token.size() > 6 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using " << (uint16_t)num << ')' << endl;
    }
    return num;
}

uint16_t evalNeg(int line, string token) {
    token.erase(0, 1);
    uint32_t num = stoi(token);
    if (token.size() > 6 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using -" << (uint16_t)num << ')' << endl;
    }
    return ~num + 1;  // Two's complement
}

uint16_t evalBin(int line, string token) {
    token.erase(0, 1);
    uint16_t num = 0;
    for (char i : token) pushBin(num, i);

    if (token.size() > 16) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t evalOct(int line, string token) {
    token.erase(0, 1);
    uint16_t num = 0;
    for (char i : token) pushOct(num, i);

    if (token.size() > 16) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t evalHex(int line, string token) {
    token.erase(0, 1);
    uint16_t num = 0;
    for (char i : token) pushHex(num, i);

    if (token.size() > 16) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t evalChar(int line, string token) {
    token.erase(0, 1);
    token.pop_back();

    if (token.size() == 2) {
        if (token[0] == '\'') {
            switch (token[1]) {
                case '0': return '\0';
                case 'a': return '\a';
                case 'b': return '\b';
                case 't': return '\t';
                case 'n': return '\n';
                case 'v': return '\v';
                case 'f': return '\f';
                case 'r': return '\r';
                case 'e': return 27;
                case '\'': return '\'';
                case '\"': return '\"';
                default:
                    lerror(line) << "Invalid char '" << token << "', using '" << token[1] << '\'' << endl;
                    return token[1];
            }
        } else {
            lerror(line) << "Char is too long, using " << token[1] << endl;
            return token[1];
        }
    } else if (token.size() == 1) {
        return token[0];
    } else if (token.size() == 0) {
        lerror(line) << "Empty char, using '\0'" << endl;
        return '\0';
    } else {
        lerror(line) << "Char is too long, using " << token[0] << endl;
        return token[0];
    }
}

pair<uint16_t, int> evalArr(int line, string token) {
    return {0, 0};
}

vector<uint16_t> evalStr(int line, string token) {
    return vector<uint16_t>(0);
}