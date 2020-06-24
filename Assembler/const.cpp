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

bool isPos(string str) {
    if (str[0] != '+') return false;

    for (unsigned i = 1; i < str.size(); i++) {
        if (!isDec(str[i])) return false;
    }

    return true;
}

bool isNeg(string str) {
    for (unsigned i = 1; i < str.size(); i++) {
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

inline static void pushBin(u16& n, char c) {
    n <<= 1;
    if (c == '1') n |= 1;
}

inline static void pushOct(u16& n, char c) {
    n <<= 3;
    n |= (c - '0');
}

inline static void pushHex(u16& n, char c) {
    n <<= 4;
    if (isDec(c)) {
        n |= (c - '0');
    } else if ('a' <= c && c <= 'f') {
        n |= (c - 'a' + 10);
    } else {  // if ('A' <= c && c <= 'F')
        n |= (c - 'A' + 10);
    }
}

u16 evalDec(int line, string str) {
    u16 num = stoi(str);
    if (str.size() > 5 || num >= (1 << 16)) {
        lerror(line) << "Number is too large (returning " << (uint16_t)num << ')' << endl;
    }
    return num;
}

u16 evalPos(int line, string str) {
    str.erase(0, 1);
    u32 num = stoi(str);
    if (str.size() > 6 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using " << (uint16_t)num << ')' << endl;
    }
    return num;
}

u16 evalNeg(int line, string str) {
    str.erase(0, 1);
    u32 num = stoi(str);
    if (str.size() > 6 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using -" << (uint16_t)num << ')' << endl;
    }
    return ~num + 1;  // Two's complement
}

u16 evalBin(int line, string str) {
    str.erase(0, 1);
    u16 num = 0;
    for (char i : str) pushBin(num, i);

    if (str.size() > 16) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

u16 evalOct(int line, string str) {
    str.erase(0, 1);
    u16 num = 0;
    for (char i : str) pushOct(num, i);

    if (str.size() > 7 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

u16 evalHex(int line, string str) {
    str.erase(0, 1);
    u16 num = 0;
    for (char i : str) pushHex(num, i);

    if (str.size() > 4) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

u16 evalDecImm(int line, string str) {
    u32 num = stoi(str);
    if (str.size() > 3 || num >= (1 << 7)) {
        lerror(line) << "Number is too large (returning " << (u32)num << ')' << endl;
    }
    return num;
}

u16 evalPosImm(int line, string str) {
    str.erase(0, 1);
    u32 num = stoi(str);
    if (str.size() > 3 || num >= (1 << 6)) {
        lerror(line) << "Number is too large (using " << (u16)num << ')' << endl;
    }
    return num;
}

u16 evalNegImm(int line, string str) {
    str.erase(0, 1);
    u32 num = stoi(str);
    if (str.size() > 3 || num >= (1 << 6)) {
        lerror(line) << "Number is too large (using -" << (u16)num << ')' << endl;
    }
    return ~num + 1;  // Two's complement
}

u16 evalBinImm(int line, string str) {
    str.erase(0, 1);
    u16 num = 0;
    for (char i : str) pushBin(num, i);

    if (str.size() > 7) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

u16 evalOctImm(int line, string str) {
    str.erase(0, 1);
    u16 num = 0;
    for (char i : str) pushOct(num, i);

    if (str.size() > 4 || num >= (1 << 7)) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

u16 evalHexImm(int line, string str) {
    str.erase(0, 1);
    u16 num = 0;
    for (char i : str) pushHex(num, i);

    if (str.size() > 3 || num >= (1 << 7)) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

u16 evalChar(int line, string str) {
    str.erase(0, 1);
    str.pop_back();

    if (str.size() == 2) {
        if (str[0] == '\'') {
            switch (str[1]) {
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
                    lerror(line) << "Invalid char '" << str << "', using '" << str[1] << '\'' << endl;
                    return str[1];
            }
        } else {
            lerror(line) << "Char is too long, using " << str[0] << endl;
            return str[1];
        }
    } else if (str.size() == 1) {
        return str[0];
    } else if (str.size() == 0) {
        lerror(line) << "Empty char, using '\0'" << endl;
        return '\0';
    } else {
        lerror(line) << "Char is too long, using " << str[0] << endl;
        return str[0];
    }
}

static pair<string, string> split(string str) {
    unsigned i = 0;
    while (str[i != ',']) {
        i++;
    }

    return pair<string, string>(str.substr(0, i - 1), str.substr(i + 1));
}

pair<u16, u16> evalArr(int line, string str) {
    str.erase(0, 1);
    str.pop_back();
    int val, len;
    return { 0, 0 }
}

vector<u16> evalStr(int line, string str) {
    return vector<u16>(0);
}