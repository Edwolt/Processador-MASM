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

#define RX  REGS[calcRX()]
#define RY  REGS[calcRY()]
#define RZ  REGS[calcRZ()]
#define SP  REGS[14]
#define AUX REGS[15]

void Processor::print(string str, int regCount) {
    cout << str;
    if (regCount >= 1) cout << " r" << calcRX() << '=' << RX;
    if (regCount >= 2) cout << " r" << calcRY() << '=' << RY;
    if (regCount >= 3) cout << " r" << calcRZ() << '=' << RZ;
    cout << endl;
}

void Processor::print(string str, int regCount, string end) {
    cout << str;
    if (regCount >= 1) cout << " r" << calcRX() << '=' << RX;
    if (regCount >= 2) cout << " r" << calcRY() << '=' << RY;
    if (regCount >= 3) cout << " r" << calcRZ() << '=' << RZ;
    cout << end;
}

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
        }

        pair<bool, u16> params = jifParams();
        bool a = (AUX & 0x000F) & params.second;

        if (params.first) a = !a;
        if (a) {
            numJumpsExecuted++;
            u16 aux = PC;
            PC = RX;
            AUX = aux;

            cout << "jumped(" << (params.first ? '!' : ' ') << params.second << ")";
            print(" ", 1);
        } else {
            if (isNoop()) {
                print("noop", 0);
            } else {
                cout << "jump(" << (params.first ? '!' : ' ') << params.second << ")";
                print(" ", 1);
            }
        }

    } else if (opcode == CMP) {
        print("cmp", 2, "");

        bool e, l, p, m;
        e = RX == RY;
        l = RX < RY;
        p = (RX != 0 ? !(RX & 0x8000) : false);
        m = RX & 0x8000;

        AUX = 0;
        if (e) AUX |= 8;
        if (l) AUX |= 4;
        if (p) AUX |= 2;
        if (m) AUX |= 1;

        cout << " -> " << AUX << endl;

    } else if (opcode == LOADSTORE) {
        if (!getBitAfterOpcode()) {  // Load
            print("load", 2, "");

            RX = memory[RY];
            cout << " -> " << RX << endl;
        } else {  // Store
            print("store", 2);

            memory[RX] = RY;
        }

    } else if (opcode == INOUT) {
        u16 address = ((IR << 5) & 0xFFFF) >> 13;
        if (!getBitAfterOpcode()) {
            cout << "in(" << address << ")";
            print(" ", 2, "");

            u16 val;
            bool attribute = io->in(address, RY, RX, val, delay);
            if (attribute) {
                RX = val;
                cout << " -> " << RX << endl;
            } else {
                cout << endl;
            }
        } else {
            cout << "out(" << address << ")";
            print("", 2);

            io->out(address, RY, RX);
        }

    } else if (opcode == MOVE) {
        print("move", 2);
        RX = RY;

    } else if (opcode == SET) {
        print("set", 1, " ");
        RX = memory[PC++];
        cout << memory[PC - 1] << endl;

    } else if (opcode == ADDISUBI) {
        u16 imm = getImm();
        if (!getBitAfterOpcode()) {  // addi
            string str = " ";
            str += to_string(imm);
            print("addi", 1, str);
            cout << endl;

            RX += imm;
        } else {  // subi
            string str = " ";
            str += to_string(imm);
            print("subi", 1, str);
            cout << endl;

            RX -= getImm();
        }

    } else if (opcode == ADD) {
        print("add", 3);

        u32 val = RY;
        val += RZ;

        RX = val;
        AUX = val >> 16;

    } else if (opcode == SUB) {
        print("sub", 3);

        u32 val = RY;
        val -= RZ;

        RX = val;
        AUX = val >> 16;

    } else if (opcode == MUL) {
        print("mul", 3);

        u32 val = RY;
        val *= RZ;

        RX = val;
        AUX = val >> 16;

    } else if (opcode == DIV) {
        print("div", 3);

        RX = RY / RZ;
        AUX = RY % RZ;

    } else if (opcode == SHIFT) {
        u16 params = shiftParams();
        if (params == 0) {  // shiftl0
            print("shiftl0", 2);
            RX = RX << (RY & 0x000F);
        } else if (params == 2) {  // shiftr0
            print("shiftr0", 2);
            RX = RX >> (RY & 0x000F);
        } else if (params == 1) {  // shiftl1
            print("shiftl1", 2);
            RX = ~((~RX) << (RY & 0x000F));
        } else if (params == 3) {  // shiftr1
            print("shiftr1", 2);
            RX = ~((~RX) >> (RY & 0x000F));
        } else if (params == 4 || params == 5) {  // rotl
            print("rotl", 2);
            RX = (RX << (RY & 0x000F)) | (RX >> (16 - (RY & 0x000F)));
        } else if (params == 6 || params == 7) {  // rotr
            print("rotr", 2);
            RX = (RX >> (RY & 0x000F)) | (RX << (16 - (RY & 0x000F)));
        }

    } else if (opcode == AND) {
        print("and", 3);
        RX = RY & RZ;

    } else if (opcode == OR) {
        print("or", 3);
        RX = RY | RZ;

    } else if (opcode == XOR) {
        print("xor", 3);
        RX = RY ^ RZ;

    } else if (opcode == NOT) {
        print("not", 3);
        RX = ~RY;
    }

    numInstructions++;
    if (!isNoop()) numExecuted++;
}
