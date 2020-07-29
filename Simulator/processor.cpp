#include "processor.hpp"

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

#define RX  REGS[IR & 0x000F]
#define RY  REGS[(IR >> 4) & 0x000F]
#define RZ  REGS[(IR >> 8) & 0x000F]
#define SP  REGS[14]
#define AUX REGS[15]

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

inline static u16 getBitAfterOpcode(u16 num) { return (num << 4) >> 15; }
inline static bool isNoop(u16 num) { return (num << 7) == 0; }
inline static u16 getOpcode(u16 num) { return (num >> 12) & 0x000F; }

struct Processor {
    vector<u16> memory;
    u16 REGS[16];
    u16 PC;
    u16 IR;
    bool halted = false;

    unsigned long long numInstructions = 0;
    unsigned long long numExecuted = 0;
    unsigned long long numJumps = 0;
    unsigned long long numJumpsExecuted = 0;

    Processor(string path) {
        memory = readBinary(path);

        for (int i = 0; i < 16; i++) REGS[i] = 0;
        SP = MEM_DEPTH - 1;
        PC = 0;
        IR = memory[PC];
    }

    void nextStep() {
        IR = memory[PC++];
        u16 opcode = getOpcode(IR);

        if (opcode == JIF) {
            if (!isNoop(IR)) numJumps++;

            pair<bool, u16> params = jifParams(IR);
            bool a = (AUX & 0x000F) & params.second;
            if (params.first) a = !a;
            if (a) {
                numJumpsExecuted++;
                AUX = PC;
                PC = RX;
            }

        } else if (opcode == CMP) {
            bool e, l, p, m;
            e = RX == RY;
            l = RX < RY;
            p = (RX != 0 ? !(RX >> 15) : false);
            m = RX >> 15;

            AUX = 0;
            if (e) AUX |= 0xb1000;
            if (l) AUX |= 0xb0100;
            if (p) AUX |= 0xb0010;
            if (m) AUX |= 0xb0001;

        } else if (opcode == LOADSTORE) {
            if (!getBitAfterOpcode(IR)) {  // Load
                RX = memory[RY];
            } else {  // Store
                memory[RX] = RY;
            }

        } else if (opcode == INOUT) {
            if (!getBitAfterOpcode(IR)) {  // In

                //TODO
            } else {  // Out

                // TODO
            }

        } else if (opcode == MOVE) {
            RX = RY;

        } else if (opcode == SET) {
            RX = memory[PC++];

        } else if (opcode == ADDISUBI) {
            // TODO

        } else if (opcode == ADD) {
            u32 val = RY;
            val += RZ;

            RX = val;
            AUX = val >> 16;

        } else if (opcode == SUB) {
            u32 val = RY;
            val -= RZ;

            RX = val;
            AUX = val >> 16;

        } else if (opcode == MUL) {
            u32 val = RY;
            val *= RZ;

            RX = val;
            AUX = val >> 16;

        } else if (opcode == DIV) {
            RX = RY / RZ;
            AUX = RY % RZ;

        } else if (opcode == SHIFT) {
            // TODO

        } else if (opcode == AND) {
            RX = RY & RZ;

        } else if (opcode == OR) {
            RX = RY | RZ;

        } else if (opcode == XOR) {
            RX = RY ^ RZ;

        } else if (opcode == NOT) {
            RX = ~RY;
        }

        numInstructions++;
        if (!isNoop(IR)) numExecuted++;
    }
};

Processor* createProcessor(string path) {
    return new Processor(path);
}

void deleteProcessor(Processor* processor) {
    delete processor;
}

void next(Processor* processor) {
    processor->nextStep();
}

bool hasNext(Processor* processor) {
    return !processor->halted;
}

unsigned long long numInstructions(Processor* processor) {
    return processor->numInstructions;
}

unsigned long long numExecuted(Processor* processor) {
    return processor->numExecuted;
}

unsigned long long numJumps(Processor* processor) {
    return processor->numJumps;
}

unsigned long long numJumpsExecuted(Processor* processor) {
    return processor->numExecuted;
}