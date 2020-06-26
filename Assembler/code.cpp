#include "code.hpp"

// TODO Compare with many strings can be more efficient using map

/**
 * Compare two string until the first reach a opening parenthesis
 */
inline static bool cmp2(string a, string b) {
    if (a.size() <= b.size()) return false;
    unsigned i;
    for (i = 0; a[i] != '('; i++) {
        if (i >= b.size() || a[i] != b[i]) return false;
    }

    return a[i] == '(';
}

inline static string noSpace(string str) {
    string newStr;
    for (char i : str) {
        if (!isSpace(i)) newStr.push_back(i);
    }
    return newStr;
}

typedef pair<u16, CodeType> puc;
pair<u16, CodeType> createCode(int line, string str) {
    if (cmp2(str, "jif")) {  // jump
        string backup = str;
        str.erase(0, 4);
        str.pop_back();

        str = noSpace(str);

        if (str.size() != 5) {
            lwarning(line) << '`' << backup << "` was considered a label, but it really looks like jif instruction" << endl;
            return puc(0, NOT);
        }

        u16 arg = 0;
        for (char i : str) {
            arg <<= 1;
            if (i == '1') {
                arg |= 1;
            }
        }
        arg <<= 7;

        u16 instruction = 0x0000 | arg;
        return puc(instruction, RX);
    } else if (str == "noop") {
        return puc(0x0000, NOOP);
    } else if (str == "jn") {
        return puc(0x0000, RX);
    } else if (str == "j") {
        return puc(0x0800, RX);
    } else if (str == "je") {
        return puc(0x0400, RX);
    } else if (str == "jne") {
        return puc(0x0C00, RX);
    } else if (str == "jl") {
        return puc(0x0200, RX);
    } else if (str == "jg") {
        return puc(0x0E00, RX);
    } else if (str == "jle") {
        return puc(0x0600, RX);
    } else if (str == "jge") {
        return puc(0x0A00, RX);
    } else if (str == "jz") {
        return puc(0x0980, RX);
    } else if (str == "jnz") {
        return puc(0x0180, RX);
    } else if (str == "jp") {
        return puc(0x0100, RX);
    } else if (str == "jpz") {
        return puc(0x0880, RX);
    } else if (str == "jm") {
        return puc(0x0080, RX);
    } else if (str == "jmz") {
        return puc(0x0900, RX);
    } else if (str == "cmp") {  // cmp
        return puc(0x1000, RY);
    } else if (str == "load") {  // load store
        return puc(0x2000, RY);
    } else if (str == "store") {
        return puc(0x2800, RY);
    } else if (str == "in") {  // in out
        return puc(0x3000, INOUT);
    } else if (str == "out") {
        return puc(0x3800, INOUT);
    } else if (str == "move") {  // move
        return puc(0x4000, RY);
    } else if (str == "set") {  // set
        return puc(0x5000, SET);
    } else if (str == "addi") {  // addi subi
        return puc(0x6000, IMM);
    } else if (str == "subi") {
        return puc(0x6800, IMM);
    } else if (str == "add") {  // add
        return puc(0x7000, RZ);
    } else if (str == "sub") {  // sub
        return puc(0x8000, RZ);
    } else if (str == "mul") {  // mul
        return puc(0x9000, RZ);
    } else if (str == "div") {  // div
        return puc(0xA000, RZ);
    } else if (cmp2(str, "shift")) {  // shift
        string backup = str;
        str.erase(0, 6);
        str.pop_back();

        str = noSpace(str);

        if (str.size() != 3) {
            lwarning(line) << '`' << backup << "` was considered a label, but it really looks like shift instruction" << endl;
            return puc(0, NOT);
        }

        u16 arg = 0;
        for (char i : str) {
            arg <<= 1;
            if (i == '1') {
                arg |= 1;
            }
        }
        arg <<= 9;

        u16 instruction = 0xB000 | arg;
        return puc(instruction, RY);
    } else if (str == "shiftl0") {
        return puc(0xB000, RY);
    } else if (str == "shiftr0") {
        return puc(0xB400, RY);
    } else if (str == "shiftl1") {
        return puc(0xB200, RY);
    } else if (str == "shiftr1") {
        return puc(0xB600, RY);
    } else if (str == "rotl") {
        return puc(0xB800, RY);
    } else if (str == "rotr") {
        return puc(0xBC00, RY);

    } else if (str == "and") {  // and
        return puc(0xC000, RZ);
    } else if (str == "or") {  // or
        return puc(0xD000, RZ);
    } else if (str == "xor") {  // xor
        return puc(0xE000, RZ);
    } else if (str == "not") {  // not
        return puc(0xF000, RY);
    } else {
        return puc(0, NOT);
    }
}

u16 createRegister(int line, string str) {
    if (str == "r0") {
        return 0x0000;
    } else if (str == "r1") {
        return 0x0001;
    } else if (str == "r2") {
        return 0x0002;
    } else if (str == "r3") {
        return 0x0003;
    } else if (str == "r4") {
        return 0x0004;
    } else if (str == "r5") {
        return 0x0005;
    } else if (str == "r6") {
        return 0x0006;
    } else if (str == "r7") {
        return 0x0007;
    } else if (str == "r8") {
        return 0x0008;
    } else if (str == "r9") {
        return 0x0009;
    } else if (str == "r10" || str == "ra") {
        return 0x000A;
    } else if (str == "r11" || str == "rb") {
        return 0x000B;
    } else if (str == "r12" || str == "rc") {
        return 0x000C;
    } else if (str == "r13" || str == "rd") {
        return 0x000D;
    } else if (str == "aux" || str == "re") {
        return 0x000E;
    } else if (str == "sp" || str == "rf") {
        return 0x000F;
    } else {
        lerror(line) << "Invalid register `" << str << "` (using aux)" << endl;
        return 0x000E;
    }
}