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
            cout << "jumped(" << (params.first ? '!' : ' ') << params.second << ") " << RX << endl;
        } else {
            cout << "jump(" << (params.first ? '!' : ' ') << params.second << ") " << RX << endl;
        }

    } else if (opcode == CMP) {
        cout << "cmp " << RX << ' ' << RY << endl;

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
            cout << "load " << RX << ' ' << RY << endl;

            RX = memory[RY];
        } else {  // Store
            cout << "store " << RX << ' ' << RY << endl;

            memory[RX] = RY;
        }

    } else if (opcode == INOUT) {
        u16 address = ((IR << 5) & 0xFFFF) >> 13;
        if (!getBitAfterOpcode()) {
            cout << "in(" << address << ") " << RX << ' ' << RY << endl;

            u16 val;
            bool attribute = io->in(address, RY, RX, val, delay);
            if (attribute) RX = val;
        } else {
            cout << "out(" << address << ") " << RX << ' ' << RY << endl;

            io->out(address, RY, RX);
        }

    } else if (opcode == MOVE) {
        cout << "move " << RX << ' ' << RY << endl;

        RX = RY;

    } else if (opcode == SET) {
        cout << "set " << RX << ' ';
        RX = memory[PC++];
        cout << memory[PC - 1] << endl;

    } else if (opcode == ADDISUBI) {
        u16 imm = getImm();
        if (!getBitAfterOpcode()) {  // addi
            cout << "addi " << RX << ' ' << imm << endl;

            RX += imm;
        } else {  // subi
            cout << "subi " << RX << ' ' << imm << endl;

            RX -= getImm();
        }

    } else if (opcode == ADD) {
        cout << "add " << RX << ' ' << RY << ' ' << RZ << endl;

        u32 val = RY;
        val += RZ;

        RX = val;
        AUX = val >> 16;

    } else if (opcode == SUB) {
        cout << "sub " << RX << ' ' << RY << ' ' << RZ << endl;

        u32 val = RY;
        val -= RZ;

        RX = val;
        AUX = val >> 16;

    } else if (opcode == MUL) {
        cout << "mul " << RX << ' ' << RY << ' ' << RZ << endl;

        u32 val = RY;
        val *= RZ;

        RX = val;
        AUX = val >> 16;

    } else if (opcode == DIV) {
        cout << "div " << RX << ' ' << RY << ' ' << RZ << endl;

        RX = RY / RZ;
        AUX = RY % RZ;

    } else if (opcode == SHIFT) {
        u16 params = shiftParams();
        if (params == 0) {  // shiftl0
            cout << "shiftl0 " << RX << ' ' << RY << endl;
            RX = RX << (RY & 0x000F);
        } else if (params == 2) {  // shiftr0
            cout << "shiftr0 " << RX << ' ' << RY << endl;
            RX = RX >> (RY & 0x000F);
        } else if (params == 1) {  // shiftl1
            cout << "shiftl1 " << RX << ' ' << RY << endl;
            RX = ~((~RX) << (RY & 0x000F));
        } else if (params == 3) {  // shiftr1
            cout << "shiftr1" << RX << ' ' << RY << endl;
            RX = ~((~RX) >> (RY & 0x000F));
        } else if (params == 4 || params == 5) {  // rotl
            RX = (RX << (RY & 0x000F)) | (RX >> (16 - (RY & 0x000F)));
            cout << "rotl " << RX << ' ' << RY << endl;
        } else if (params == 6 || params == 7) {  // rotr
            RX = (RX >> (RY & 0x000F)) | (RX << (16 - (RY & 0x000F)));
            cout << "rotr " << RX << ' ' << RY << endl;
        }

    } else if (opcode == AND) {
        cout << "and " << RX << ' ' << RY << ' ' << RZ << endl;
        RX = RY & RZ;

    } else if (opcode == OR) {
        cout << "or " << RX << ' ' << RY << ' ' << RZ << endl;
        RX = RY | RZ;

    } else if (opcode == XOR) {
        cout << "xor " << RX << ' ' << RY << ' ' << RZ << endl;
        RX = RY ^ RZ;

    } else if (opcode == NOT) {
        cout << "not " << RX << ' ' << RY << endl;
        RX = ~RY;
    }

    numInstructions++;
    if (!isNoop()) numExecuted++;
}
