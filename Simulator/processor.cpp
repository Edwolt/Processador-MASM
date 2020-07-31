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
    if (delay < 0) {
        return;
    }
    if (delay > 0) {
        delay--;
        return;
    }

    IR = memory[PC++];
    u16 opcode = getOpcode();

    if (opcode == JIF) {
        if (!isNoop()) {
            numJumps++;
        } else {
            cout << "noop" << endl;
        }

        pair<bool, u16> params = jifParams();
        bool a = (AUX & 0x000F) & params.second;
        if (params.first) a = !a;
        if (a) {
            numJumpsExecuted++;
            AUX = PC;
            PC = RX;
            cout << "jumped to " << RX << endl;
        } else {
            cout << "jump" << endl;
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

        cout << "cmp" << endl;

    } else if (opcode == LOADSTORE) {
        if (!getBitAfterOpcode()) {  // Load
            RX = memory[RY];
            cout << "load" << endl;
        } else {  // Store
            memory[RX] = RY;
            cout << "store" << endl;
        }

    } else if (opcode == INOUT) {
        u16 address = ((IR << 5) & 0xFFFF) >> 13;
        if (!getBitAfterOpcode()) {
            u16 val;
            bool attribute = io->in(address, RX, RY, val, delay);
            if (attribute) RX = val;
            cout << "in" << endl;
        } else {
            io->out(address, RX, RY);
            cout << "out" << endl;
        }

    } else if (opcode == MOVE) {
        RX = RY;
        cout << "move" << endl;

    } else if (opcode == SET) {
        RX = memory[PC++];
        cout << "set" << endl;

    } else if (opcode == ADDISUBI) {
        if (!getBitAfterOpcode()) {  // addi
            cout << "addi" << endl;
            RX += getImm();
        } else {  // subi
            cout << "subi" << endl;
            RX -= getImm();
        }

    } else if (opcode == ADD) {
        u32 val = RY;
        val += RZ;

        RX = val;
        AUX = val >> 16;
        cout << "add" << endl;

    } else if (opcode == SUB) {
        u32 val = RY;
        val -= RZ;

        RX = val;
        AUX = val >> 16;
        cout << "sub" << endl;

    } else if (opcode == MUL) {
        u32 val = RY;
        val *= RZ;

        RX = val;
        AUX = val >> 16;
        cout << "mul" << endl;

    } else if (opcode == DIV) {
        RX = RY / RZ;
        AUX = RY % RZ;
        cout << "div" << endl;

    } else if (opcode == SHIFT) {
        u16 params = shiftParams();
        if (params == 0) {  // shiftl0
            RX = RX << (RY & 0x000F);
            cout << "shiftl0" << endl;
        } else if (params == 2) {  // shiftr0
            RX = RX >> (RY & 0x000F);
            cout << "shiftr0" << endl;
        } else if (params == 1) {  // shiftl1
            RX = ~((~RX) << (RY & 0x000F));
            cout << "shiftl1" << endl;
        } else if (params == 3) {  // shiftr1
            RX = ~((~RX) >> (RY & 0x000F));
            cout << "shiftr1" << endl;
        } else if (params == 4 || params == 5) {  // rotl
            RX = (RX << (RY & 0x000F)) | (RX >> (16 - (RY & 0x000F)));
            cout << "rotl" << endl;
        } else if (params == 6 || params == 7) {  // rotr
            RX = (RX >> (RY & 0x000F)) | (RX << (16 - (RY & 0x000F)));
            cout << "rotr" << endl;
        }

    } else if (opcode == AND) {
        RX = RY & RZ;
        cout << "and" << endl;

    } else if (opcode == OR) {
        RX = RY | RZ;
        cout << "or" << endl;

    } else if (opcode == XOR) {
        RX = RY ^ RZ;
        cout << "xor" << endl;

    } else if (opcode == NOT) {
        RX = ~RY;
        cout << "not" << endl;
    }

    numInstructions++;
    if (!isNoop()) numExecuted++;
}
