#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "util.hpp"

using namespace std;

bool isNum(string str);
bool isInvalidNum(string str);
u16 evalNum(int line, string str);
u16 evalNumImm(int line, string str);

pair<u16, u16> evalArr(int line, string str);
vector<u16> evalStr(int line, string str);