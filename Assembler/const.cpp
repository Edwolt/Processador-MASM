#include "const.hpp"

inline static bool isDec(char c) { return '0' <= c && c <= '9'; }
inline static bool isBin(char c) { return c == '0' || c == '1'; }
inline static bool isHex(char c) { return isDec(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }

inline static void pushBin(uint16_t& n, char c) {
    n <<= 1;
    if (c == '1') n |= 1;
}

inline static void pushOctal(uint16_t& n, char c) {
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

uint16_t createDecimal(string token) {
    uint32_t num = stoi(token);
    if (token.size() > 5 || num >= (1 << 16)) {
        uint16_t num = stoi(token);
        lerror(-1) << "Number is too large (returning " << num << ')' << endl;
    }
    return num;
}

uint16_t createPositive(string token) {
    token.erase(0, 1);
    uint32_t num = stoi(token);
    if (token.size() > 6 || num >= (1 << 15)) {
        uint16_t num = stoi(token);
        lerror(-1) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t createNegative(string token) {
    token.erase(0, 1);
    uint32_t num = stoi(token);
    if (token.size() > 6 || num >= (1 << 15)) {
        lerror(-1) << "Number is too large (using -" << num << ')' << endl;
    }
    return ~num + 1;  // Two's complement
}

uint16_t createBinary(string token) {
    token.erase(0, 1);
    uint16_t num = 0;
    for (int i : token) pushBin(num, i);

    if (token.size() > 16) {
        lerror(-1) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t createOctal(string token) {
    token.erase(0, 1);
    uint16_t num = 0;
    for (int i : token) pushOctal(num, i);

    if (token.size() > 16) {
        lerror(-1) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t createHexadecimal(string token) {
    token.erase(0, 1);
    uint16_t num = 0;
    for (int i : token) pushHex(num, i);

    if (token.size() > 16) {
        lerror(-1) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

uint16_t createChar(string token) {
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
                    lerror(-1) << "Invalid char '" << token << "', using '" << token[1] << '\'' << endl;
                    return token[1];
            }
        } else {
            lerror(-1) << "Char is too long" << endl;
        }
    } else if (token.size() == 1) {
        return token[0];
    } else if (token.size() == 0) {
        lerror(-1) << "Empty char" << endl;
    } else {
        lerror(-1) << "Char is too long" << endl;
    }
}

pair<uint16_t, int> createArray(string token) {
}

vector<uint16_t> createString(string token) {
}