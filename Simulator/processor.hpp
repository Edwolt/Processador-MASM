#pragma once

#include <string>
#include <vector>

#include "files.hpp"
#include "util.hpp"

using namespace std;

typedef unsigned long long ull;

struct Processor {
    vector<u16> memory;
    u16 REGS[16];
    u16 PC;
    u16 IR;
    bool halted = false;

    inline pair<bool, u16> jifParams() {
        bool n = IR & (1 << 11);
        u16 elpm = (IR >> 23) & 0x00F;
        return pair<bool, u16>(n, elpm);
    }
    inline trio<bool, bool, bool> shiftParams() {
        bool t = IR & (1 << 11);
        bool d = IR & (1 << 10);
        bool b = IR & (1 << 9);
        return trio<bool, bool, bool>(t, d, b);
    }

    inline u16 getBitAfterOpcode() { return (IR << 4) >> 15; }
    inline bool isNoop() { return (IR << 7) == 0; }
    inline u16 getOpcode() { return (IR >> 12) & 0x000F; }

    ull numInstructions;
    ull numExecuted;
    ull numJumps;
    ull numJumpsExecuted;

    Processor(string path);

    void next();
    bool hasNext();
};