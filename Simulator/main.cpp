// #include <GL/glut.h>  // Linux

#include <iostream>
#include <string>
#include <vector>

#include "files.hpp"
#include "util.hpp"

using namespace std;

enum Opcodes {
    JIF = 0x0,
    CMP = 0x1,
    LOADSTORE = 0x2,
    INOUT = 0x3,
    MOVE = 0x4,
    SET = 0x5,
    ADDISUBI = 0x6,
    ADD = 0x7,
    SUB = 0x8,
    MUL = 0x9,
    DIV = 0xA,
    SHIFT = 0xB,
    AND = 0xC,
    OR = 0xD,
    XOR = 0xE,
    NOT = 0xF
};

enum Regs {
    R1 = 0,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    RA,
    RB,
    RC,
    RD,
    SP,
    AUX
};

inline static pair<bool, u16> jifParams(u16 num) {
    bool n = num & (1 << 11);
    u16 elpm = (num >> 23) & 0x00F;
    return pair<bool, u16>(n, elpm);
}
inline static trio<bool, bool, bool> shiftParams(u16 num) {
    bool t = num & (1 << 11);
    bool d = num & (1 << 10);
    bool b = num & (1 << 9);
    return trio<bool, bool, bool>(t, d, b);
}

inline static u16 calcRX(u16 num) { return num & 0x000F; }
inline static u16 calcRY(u16 num) { return (num >> 4) & 0x000F; }
inline static u16 calcRZ(u16 num) { return (num >> 8) & 0x000F; }



inline static u16 getOpcode(u16 num) { return (num >> 12) & 0x000F; }
inline static u16 getBitAfterOpcode(u16 num) { return (num << 4) >> 15; }

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        cout << "Missing source" << endl;
        return EXIT_FAILURE;
    }

    unsigned long long counter = 0, a = 0, jumps = 0, jumps2 = 0;

    u16 regs[16];
    for (int i = 0; i < 16; i++) regs[i] = 0;
    regs[15] = MEM_DEPTH - 1;

    string path(argv[1]);
    vector<u16> memory = readBinary(path);

    u16 pc = 0;
    while (true) {
        u16 instruction = memory[pc++];
        switch (getOpcode(instruction)) {
            case JIF:
                if (instruction == 0) break;
                jumps++;

                pair<bool, u16> params = jifParams(instruction);
                bool a = (regs[AUX] & 0x000F) & params.second;
                if (params.first) a = !a;
                if (a) {
                    jumps2++;
                    regs[AUX] = pc;
                    pc = regs[calcRX(instruction)];
                }
                break;

            case CMP:
                u16 rx = getRX(regs, instruction);
                u16 ry = getRY(regs, instruction);

                bool e, l, p, m;
                e = rx == ry;
                l = rx < ry;
                p = (rx != 0 ? !(rx >> 15) : false);
                m = rx >> 15;

                regs[AUX] = 0;
                if (e) regs[AUX] |= 0xb1000;
                if (l) regs[AUX] |= 0xb0100;
                if (p) regs[AUX] |= 0xb0010;
                if (m) regs[AUX] |= 0xb0001;

                break;

            case LOADSTORE:
                break;

            case INOUT:
                // TODO
                break;

            case MOVE:
                break;

            case SET:
                break;

            case ADDISUBI:
                break;

            case ADD:
                u32 aux = regs[endRY(instruction)];
                aux += regs[endRZ(instruction)];

                u16 value = aux;
                u16 overflow = aux >> 16;

                regs[endRX(instruction)] = value;
                regs[AUX] = overflow;
                break;

            case SUB:
                u32 aux = getRY(regs, instruction);
                aux -= getRZ(regs, instruction);

                u16 value = aux;
                u16 overflow = aux >> 16;

                regs[endRX(instruction)] = value;
                regs[AUX] = overflow;
                break;

            case MUL:
                u32 aux = getRY(regs, instruction);
                aux *= getRZ(regs, instruction);

                u16 value = aux;
                u16 overflow = aux >> 16;

                regs[endRX(instruction)] = value;
                regs[AUX] = overflow;
                break;

            case DIV:
                u16 ry = getRY(regs, instruction);
                u16 rz = getRZ(regs, instruction);
                regs[endRX(instruction)] = ry / rz;
                regs[endRX(instruction)] = ry % rz;
                break;

            case SHIFT:
                break;

            case AND:
                regs[endRX(instruction)] = getRY(regs, instruction) & getRZ(regs, instruction);
                break;

            case OR:
                regs[endRX(instruction)] = getRY(regs, instruction) | getRZ(regs, instruction);
                break;

            case XOR:
                regs[endRX(instruction)] = getRY(regs, instruction) ^ getRZ(regs, instruction);
                break;

            case NOT:
                regs[endRX(instruction)] = ~getRY(regs, instruction);
                break;
        }
        counter++;
        a += (getOpcode(instruction) != 0);
    }

    return EXIT_SUCCESS;
}