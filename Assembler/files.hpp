#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "util.hpp"

using namespace std;

const int MEM_DEPTH = 1 << 16;

void writeMIF(string path, vector<uint16_t> memory);

void writeBinary(string path, vector<uint16_t> memory);

void writeText(string path, vector<uint16_t> memory);
