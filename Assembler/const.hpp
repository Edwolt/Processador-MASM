#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "util.hpp"

using namespace std;

uint16_t createDecimal(string token);

uint16_t createPositive(string token);

uint16_t createNegative(string token);

uint16_t createBinary(string token);

uint16_t createOctal(string token);

uint16_t createHexadecimal(string token);

uint16_t createChar(string token);

pair<uint16_t, int> createArray(string token);

vector<uint16_t> createString(string token);