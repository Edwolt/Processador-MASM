#pragma once

#include <string>
#include <vector>

#include "files.hpp"
#include "io.hpp"
#include "util.hpp"

using namespace std;

typedef unsigned long long ull;

struct Processor {
    vector<u16> memory;
    u16 REGS[16];
    u16 PC;
    u16 IR;
    IO* io;

    inline pair<bool, u16> jifParams() {
        bool n = IR & (1 << 11);
        u16 elpm = (IR >> 23) & 0x00F;
        return pair<bool, u16>(n, elpm);
    }
    u16 shiftParams() { return (IR << 4) >> 13; }

    inline u16 getBitAfterOpcode() { return (IR << 4) >> 15; }
    inline bool isNoop() { return (IR << 7) == 0; }
    inline u16 getOpcode() { return (IR >> 12) & 0x000F; }
    inline u16 getImm() { return (IR << 5) >> 9; }

    ull numInstructions = 0;
    ull numExecuted = 0;
    ull numJumps = 0;
    ull numJumpsExecuted = 0;

    Processor() {}
    Processor(string path, IO* io);

    void next();
};