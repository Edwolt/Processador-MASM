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

Processor::Processor(string path, IO* io) {
    this->io = io;
    memory = readBinary(path);

    for (int i = 0; i < 16; i++) REGS[i] = 0;
    SP = MEM_DEPTH - 1;
    PC = 0;
    IR = memory[PC];
}

void Processor::next() {
    IR = memory[PC++];
    u16 opcode = getOpcode();

    if (opcode == JIF) {
        if (!isNoop()) numJumps++;

        pair<bool, u16> params = jifParams();
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
        if (!getBitAfterOpcode()) {  // Load
            RX = memory[RY];
        } else {  // Store
            memory[RX] = RY;
        }

    } else if (opcode == INOUT) {
        u16 address = (IR << 5) >> 13;
        if (!getBitAfterOpcode()) {
            u16 val;
            bool attribute = io->in(address, RX, RY, val);
            if (attribute) RX = val;
        } else {
            io->out(address, RX, RY);
        }

    } else if (opcode == MOVE) {
        RX = RY;

    } else if (opcode == SET) {
        RX = memory[PC++];

    } else if (opcode == ADDISUBI) {
        if (!getBitAfterOpcode()) {  // addi
            RX += getImm();
        } else {  // subi
            RX -= getImm();
        }

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
        u16 params = shiftParams();
        if (params == 0) {  // shiftl0
            RX = RX << RY;
        } else if (params == 2) {  // shiftt0
            RX = RX >> RY;
        } else if (params == 1) {  // shiftl1
            RX = ~((~RX) << RY);
        } else if (params == 3) {  // shiftr1
            RX = ~((~RX) >> RY);
        } else if (params == 4 || params == 5) {  // rotl
            RX = (RX << RY) | (RX >> (16 - RY));
        } else if (params == 6 || params == 7) {  // rotr
            RX = (RX >> RY) | (RX << (16 - RY));
        }

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
    if (!isNoop()) numExecuted++;
}
