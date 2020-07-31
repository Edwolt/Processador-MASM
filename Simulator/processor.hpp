#pragma once

#include <iostream>
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

    inline u16 calcRX() { return IR & 0x000F; }
    inline u16 calcRY() { return (IR >> 4) & 0x000F; }
    inline u16 calcRZ() { return (IR >> 8) & 0x000F; }

    /**
     * if delay < 0: never go out the delay
     * if delay = 0: execute the instruction
     * if delay > 0: delay is the number of clocks that the processor need to wait
     */
    i32 delay = 0;

    // Caution with bitshifts
    // The behavior s undefined for signed vars
    // And all bitshift return a int

    inline pair<bool, u16> jifParams() {
        bool n = IR & (1 << 11);
        u16 elpm = (IR >> 7) & 0x000F;
        return pair<bool, u16>(n, elpm);
    }
    u16 shiftParams() { return ((IR << 4) & 0xFFFF) >> 13; }

    inline u16 getBitAfterOpcode() { return ((IR << 4) & 0xFFFF) >> 15; }
    inline bool isNoop() { return (IR >> 7) == 0; }
    inline u16 getOpcode() { return (IR >> 12) & 0x000F; }
    inline u16 getImm() { return ((IR << 5) & 0xFFFF) >> 9; }

    ull numClock = 0;
    ull numInstructions = 0;
    ull numExecuted = 0;
    ull numJumps = 0;
    ull numJumpsExecuted = 0;
    ull numMemory = 0;
    ull numMemoryRandom = 0;
    ull numIO = 0;

    Processor() {}
    Processor(string path, IO* io);

    void print(string str, int regCount);
    void print(string str, int regCount, string end);

    void next();
};