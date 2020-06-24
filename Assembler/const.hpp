#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "util.hpp"

using namespace std;

inline bool isDec(char c);
inline bool isBin(char c);
inline bool isOct(char c);
inline bool isHex(char c);

bool isDec(string str);
bool isBin(string str);
bool isOct(string str);
bool isHex(string str);

uint16_t evalDec(int line, string token);
uint16_t evalPos(int line, string token);
uint16_t evalNeg(int line, string token);
uint16_t evalBin(int line, string token);
uint16_t evalOct(int line, string token);
uint16_t evalHex(int line, string token);
uint16_t evalChar(int line, string token);
pair<uint16_t, int> evalArr(int line, string token);
vector<uint16_t> evalStr(int line, string token);