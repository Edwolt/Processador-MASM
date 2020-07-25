#pragma once

#include <map>
#include <string>

#include "util.hpp"

using namespace std;

enum CodeType {
    NOT,    // Tells that the given token isn't a code token
    NOOP,   // Tells that the instruction is complete
    RX,     // Tells that the instruction need a register rx
    RY,     // Tells that the instruction need the registers rx and ry
    RZ,     // Tells that the instruction need the registers rx, ry and rz
    SET,    // Tells that the instruction is the set instruction
    INOUT,  // Tells that the instruciotn is a in or a out instruction
    IMM,    // Tells that the instruciotn is a addi or a subi instruction
};

pair<u16, CodeType> createCode(int line, string str);
u16 createRegister(int line, string str);