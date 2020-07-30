#include "code.hpp"

// TODO Pseudoinstruções

typedef pair<u16, CodeType> puc;

map<string, puc> codes = {
    {"noop", puc(0x0000, NONE)},  // Jump
    {"jn", puc(0x0000, RX)},
    {"j", puc(0x0800, RX)},
    {"je", puc(0x0400, RX)},
    {"jne", puc(0x0C00, RX)},
    {"jl", puc(0x0200, RX)},
    {"jg", puc(0x0E00, RX)},
    {"jle", puc(0x0600, RX)},
    {"jge", puc(0x0A00, RX)},
    {"jz", puc(0x0980, RX)},
    {"jnz", puc(0x0180, RX)},
    {"jp", puc(0x0100, RX)},
    {"jpz", puc(0x0880, RX)},
    {"jm", puc(0x0080, RX)},
    {"jmz", puc(0x0900, RX)},
    {"cmp", puc(0x1000, RY)},   // cmp
    {"load", puc(0x2000, RY)},  // load store
    {"store", puc(0x2800, RY)},
    {"in", puc(0x3000, INOUT)},  // in
    {"halt", puc(0x30FF, NONE)},
    {"inbutton", puc(0x31F0, RX)},
    {"insw", puc(0x32F0, RX)},
    {"inchar", puc(0x33F0, RX)},
    {"inmem", puc(0x34F0, RY)},
    {"sleep", puc(0x37F0, RX)},
    {"out", puc(0x3800, INOUT)},  // out
    {"outled", puc(0x38F0, RX)},
    {"outseg1", puc(0x39F0, RX)},
    {"outseg2", puc(0x3AF0, RX)},
    {"outpx", puc(0x3B000, RY)},
    {"outpx", puc(0x3C000, RY)},
    {"move", puc(0x4000, RY)},   // move
    {"set", puc(0x5000, SET)},   // set
    {"addi", puc(0x6000, IMM)},  // addi subi
    {"subi", puc(0x6800, IMM)},
    {"add", puc(0x7000, RZ)},  // add
    {"sub", puc(0x8000, RZ)},  // sub
    {"mul", puc(0x9000, RZ)},  // mul
    {"div", puc(0xA000, RZ)},  // div
    {"shiftl0", puc(0xB000, RY)},
    {"shiftr0", puc(0xB400, RY)},
    {"shiftl1", puc(0xB200, RY)},
    {"shiftr1", puc(0xB600, RY)},
    {"rotl", puc(0xB800, RY)},
    {"rotr", puc(0xBC00, RY)},
    {"and", puc(0xC000, RZ)},  // and
    {"or", puc(0xD000, RZ)},   // or
    {"xor", puc(0xE000, RZ)},  // xor
    {"not", puc(0xF000, RY)}   // not
};

map<string, u16> regs = {
    {"r0", 0x0000},
    {"r1", 0x0001},
    {"r2", 0x0002},
    {"r3", 0x0003},
    {"r4", 0x0004},
    {"r5", 0x0005},
    {"r6", 0x0006},
    {"r7", 0x0007},
    {"r8", 0x0008},
    {"r9", 0x0009},
    {"r10", 0x000A},
    {"r11", 0x000B},
    {"r12", 0x000C},
    {"r13", 0x000D},
    {"r14", 0x000E},
    {"r15", 0x000F},
    {"ra", 0x000A},
    {"rb", 0x000B},
    {"rc", 0x000C},
    {"rd", 0x000D},
    {"re", 0x000E},
    {"rf", 0x000F},
    {"sp", 0x000E},
    {"aux", 0x000F},
};

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
    } else if (codes.find(str) != codes.end()) {
        return codes[str];
    } else {
        return puc(0, NOT);
    }
}

u16 createRegister(int line, string str) {
    if (regs.find(str) != regs.end()) {
        return regs[str];
    } else {
        lerror(line) << "Invalid register `" << str << "` (using aux)" << endl;
        return 0x000F;
    }
}