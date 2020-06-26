#include "const.hpp"

//* ======================== *//
//* ===== Testing Char ===== *//
//* ======================== *//

inline bool isDec(char c) { return '0' <= c && c <= '9'; }
inline bool isBin(char c) { return c == '0' || c == '1'; }
inline bool isOct(char c) { return '0' <= c && c <= '7'; }
inline bool isHex(char c) { return isDec(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }

//* ========================== *//
//* ===== Testing String ===== *//
//* ========================== *//

static bool isDec(string str) {
    for (char i : str) {
        if (!isDec(i)) return false;
    }
    return true;
}

static bool isPos(string str) {
    if (str[0] != '+') return false;
    for (unsigned i = 1; i < str.size(); i++) {
        if (!isDec(str[i])) return false;
    }
    return true;
}

static bool isNeg(string str) {
    if (str[0] != '-') return false;
    for (unsigned i = 1; i < str.size(); i++) {
        if (!isDec(str[i])) return false;
    }
    return true;
}

static bool isBin(string str) {
    if (str.front() != 'b') return false;
    for (unsigned i = 1; i < str.size(); i++) {
        if (!isBin(str[i])) return false;
    }
    return true;
}

static bool isOct(string str) {
    if (str.front() != 'o') return false;
    for (unsigned i = 1; i < str.size(); i++) {
        if (!isOct(str[i])) return false;
    }
    return true;
}

static bool isHex(string str) {
    if (str.front() != 'x' && str.front() != '#') return false;
    for (unsigned i = 1; i < str.size(); i++) {
        if (!isHex(str[i])) return false;
    }
    return true;
}

//* ============================== *//
//* ===== Auxiliar Functions ===== *//
//* ============================== *//

static void trim(string& str) {
    unsigned i = 0;
    while (isSpace(str[i])) {
        i++;
    }

    str.erase(0, i);

    while (isSpace(str.back())) {
        str.pop_back();
    }
}

static pair<string, string> split(string str) {
    unsigned i = 0;
    while (i < str.size() && str[i] != ',') {
        i++;
    }

    pair<string, string> splited;
    if (i < str.size() - 1) {
        splited = pair<string, string>(str.substr(0, i), str.substr(i + 1));
        trim(splited.first);
        trim(splited.second);
    } else {
        splited = pair<string, string>(str, "");
        trim(splited.first);
    }
    return splited;
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

static u16 evalEscapedChar(int line, char c) {
    switch (c) {
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
            lerror(line) << "Invalid char '\\" << c << "' (using '" << c << "')" << endl;
            return c;
    }
}

/**
 * Removes the last and the first character
 */
inline static void removeLastAndFirst(string& str) {
    str.erase(0, 1);
    str.pop_back();
}

//* ====================== *//
//* ===== Eval Const ===== *//
//* ====================== *//

static u16 evalDec(int line, string str) {
    u16 num = stoi(str);
    if (str.size() > 5 || num >= (1 << 16)) {
        lerror(line) << "Number is too large (using " << (uint16_t)num << ')' << endl;
    }
    return num;
}

static u16 evalPos(int line, string str) {
    str[0] = ' ';
    u32 num = stoi(str);
    if (str.size() > 6 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using " << (uint16_t)num << ')' << endl;
    }
    return num;
}

static u16 evalNeg(int line, string str) {
    str[0] = ' ';
    u32 num = stoi(str);
    if (str.size() > 6 || num >= (1 << 15)) {
        lerror(line) << "Number is too large (using -" << (uint16_t)num << ')' << endl;
    }
    return ~num + 1;  // Two's complement
}

static u16 evalBin(int line, string str) {
    u16 num = 0;
    for (unsigned i = 1; i < str.size(); i++) pushBin(num, str[i]);

    if (str.size() > 16) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

static u16 evalOct(int line, string str) {
    u16 num = 0;
    for (unsigned i = 1; i < str.size(); i++) pushOct(num, str[i]);

    if (str.size() > 7 || num >= (1 << 16)) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

static u16 evalHex(int line, string str) {
    u16 num = 0;
    for (unsigned i = 1; i < str.size(); i++) pushHex(num, str[i]);

    if (str.size() > 5) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

//* ========================== *//
//* ===== Eval Immediate ===== *//
//* ========================== *//

static u16 evalDecImm(int line, string str) {
    u32 num = stoi(str);
    if (str.size() > 3 || num >= (1 << 7)) {
        lerror(line) << "Number is too large (using " << (u32)num << ')' << endl;
    }
    return num;
}

static u16 evalPosImm(int line, string str) {
    str[0] = ' ';
    u32 num = stoi(str);
    if (str.size() > 3 || num >= (1 << 6)) {
        lerror(line) << "Number is too large (using " << (u16)num << ')' << endl;
    }
    return num;
}

static u16 evalNegImm(int line, string str) {
    str[0] = ' ';
    u32 num = stoi(str);
    if (str.size() > 3 || num >= (1 << 6)) {
        lerror(line) << "Number is too large (using -" << (u16)num << ')' << endl;
    }
    return ~num + 1;  // Two's complement
}

static u16 evalBinImm(int line, string str) {
    u16 num = 0;
    for (unsigned i = 1; i < str.size(); i++) pushBin(num, str[i]);

    if (str.size() > 7) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

static u16 evalOctImm(int line, string str) {
    u16 num = 0;
    for (unsigned i = 1; i < str.size(); i++) pushOct(num, str[i]);

    if (str.size() > 4 || num >= (1 << 7)) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

static u16 evalHexImm(int line, string str) {
    u16 num = 0;
    for (unsigned i = 1; i < str.size(); i++) pushHex(num, str[i]);

    if (str.size() > 3 || num >= (1 << 7)) {
        lerror(line) << "Number is too large (using " << num << ')' << endl;
    }
    return num;
}

static u16 evalChar(int line, string str) {
    removeLastAndFirst(str);

    if (str.size() == 2 && str[0] == '\'') {
        return evalEscapedChar(line, str[0]);
    } else if (str.size() == 1) {
        return str[0];
    } else if (str.size() == 0) {
        lerror(line) << "Empty char (using '\\0')" << endl;
        return '\0';
    } else {
        lerror(line) << "Char is too long  (using '" << str[0] << "')" << endl;
        return str[0];
    }
}

//* ============================ *//
//* ===== Public Functions ===== *//
//* ============================ *//

bool isNum(string str) {
    /*
    This is less efficient than classify then eval,
    because it will be necessary to classify twice,
    but it's more readble
    */

    return str.front() == '\'' ||
           isDec(str) ||
           isPos(str) ||
           isNeg(str) ||
           isBin(str) ||
           isOct(str) ||
           isHex(str);
}

bool isInvalidNum(string str) {
    return str.front() == '+' ||
           str.front() == '-' ||
           str.front() == '#';
}

u16 evalNum(int line, string str) {
    if (str.front() == '\'') return evalChar(line, str);
    if (isDec(str)) return evalDec(line, str);
    if (isPos(str)) return evalPos(line, str);
    if (isNeg(str)) return evalNeg(line, str);
    if (isBin(str)) return evalBin(line, str);
    if (isOct(str)) return evalOct(line, str);
    if (isHex(str)) return evalHex(line, str);

    cbug << "evalNum() failed: `" << str << '`' << endl;
    return 0;
}

u16 evalNumImm(int line, string str) {
    if (str.front() == '\'') {
        u16 val = evalChar(line, str);
        if (val >= (1 << 7)) {
            lerror(line) << "The char val is too big (using " << val << ')' << endl;
        }
    }
    if (isDec(str)) return evalDecImm(line, str);
    if (isPos(str)) return evalPosImm(line, str);
    if (isNeg(str)) return evalNegImm(line, str);
    if (isBin(str)) return evalBinImm(line, str);
    if (isOct(str)) return evalOctImm(line, str);
    if (isHex(str)) return evalHexImm(line, str);

    cbug << "evalNumImm() failed: `" << str << '`' << endl;
    return 0;
}

pair<u16, u16> evalArr(int line, string str) {
    removeLastAndFirst(str);
    if (str.empty()) {
        lerror(line) << "Empty array, (using [0, 0])" << endl;
        return pair<u16, u16>(0, 0);
    }

    pair<string, string> p = split(str);

    u16 first;
    if (p.first.empty()) {
        first = 0;
    } else if (isNum(p.first)) {
        first = evalNum(line, p.first);
    } else if (isInvalidNum(p.first)) {
        lerror(line) << "Invalid Number: `" << str << '`' << endl;
        first = 0;
    } else {
        lerror(line) << "Expected a Number, get `" << str << '`' << endl;
        first = 0;
    }

    u16 second;
    if (p.second.empty()) {
        second = 0;
    } else if (isNum(p.second)) {
        second = evalNum(line, p.second);
    } else if (isInvalidNum(p.second)) {
        lerror(line) << "Invalid Number: `" << str << '`' << endl;
        first = 0;
    } else {
        lerror(line) << "Expected a Number, get `" << str << '`' << endl;
        first = 0;
    }

    return pair<u16, u16>(first, second);
}

vector<u16> evalStr(int line, string str) {
    removeLastAndFirst(str);

    vector<u16> values;
    bool escaped = false;
    for (char i : str) {
        if (escaped) {
            values.push_back(evalEscapedChar(line, i));
            escaped = false;
        } else if (i == '\\') {
            escaped = true;
        } else {
            values.push_back(i);
        }
    }

    return values;
}