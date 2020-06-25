#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "util.hpp"

using namespace std;

void writeMIF(string path, vector<u16> memory);

void writeBinary(string path, vector<u16> memory);

void writeText(string path, vector<u16> memory);
