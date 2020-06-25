#pragma once

#include <string>

#include "util.hpp"

using namespace std;

enum CodeType {
    NOT,   // Tells the parser that the given token isn't a code token
    NOOP,  // Tells the parser that the instruction is complete
    RX,    // Tells the parser that the instruction need a register rx
    RY,    // Tells the parser that the instruction need the registers rx and ry
    RZ,    // Tells the parser that the instruction need the registers rx, ry and rz
    SET,   // Tells the parser that the instruction is the set instruction
    IMM,   // Tells the parser that the instruciotn is a addi or a subi
};

pair<u16, CodeType> createCode(string str);
u16 createRegister(string str);