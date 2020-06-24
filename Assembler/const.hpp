#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "util.hpp"

using namespace std;

inline bool isDec(char c);  // Returns if the char is a deciaml digit
inline bool isBin(char c);  // Returns if the char is a binary digit
inline bool isOct(char c);  // Returns if the char is a octal digit
inline bool isHex(char c);  // Returns if the char is a hexadecimal digit

bool isDec(string str);  // Returns if the string is decimal
bool isPos(string str);  // Returns if the string is positive decimal
bool isNeg(string str);  // Returns if the string is negative decimal
bool isBin(string str);  // Returns if the string is binary number
bool isOct(string str);  // Returns if the string is octal
bool isHex(string str);  // Returns if the string is hexadecimal

u16 evalDec(int line, string str);  // `DDDDD`:             Evaluates a decimal in the string: DDDDD
u16 evalPos(int line, string str);  // `+DDDDD`:            Evaluates a positive decimal in the string
u16 evalNeg(int line, string str);  // `-DDDDD`:            Evaluates a negative decimal in the string
u16 evalBin(int line, string str);  // `bBBBBBBBBBBBBBBBB`: Evaluates a binary number in the string
u16 evalOct(int line, string str);  // `oOOOOOO`:           Evaluates a octal in the string
u16 evalHex(int line, string str);  // `xHHHH`, `#HHHH`:    Evaluates a hexadecimal in the string

u16 evalDecImm(int line, string str);  // `DDD`: Evaluates a decimal in the string with 7 bits
u16 evalPosImm(int line, string str);  // `-DD`: Evaluates a positive decimal in the string with 7 bits
u16 evalNegImm(int line, string str);  // `+DD`: Evaluates a negative decimal in the string with 7 bits
u16 evalBinImm(int line, string str);  // `bBBBBBBB`: Evaluates a binary number in the string with 7 bits
u16 evalOctImm(int line, string str);  // `oOOO`: Evaluates a octal in the string with 7 bits
u16 evalHexImm(int line, string str);  // `xHH`, `#HH`: Evaluates a hexadecimal in the string with 7 bits

u16 evalChar(int line, string str);            // Evaluates a char
pair<u16, u16> evalArr(int line, string str);  // Evaluates a array
vector<u16> evalStr(int line, string str);     // Evaluates a string