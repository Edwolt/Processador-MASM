#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "util.hpp"

using namespace std;

const int MEM_DEPTH = 1 << 16;

void writeMIF(string path, vector<u16> memory);

void writeBinary(string path, vector<u16> memory);

void writeText(string path, vector<u16> memory);
