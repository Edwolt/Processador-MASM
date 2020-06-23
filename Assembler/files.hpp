#pragma once

#include <cstdint>
#include <string>
#include <vector>

void writeMIF(string path, vector<unint_16t> memory);

void writeBinary(string path, vector<unint_16t> memory);

void writeText(string path, vector<unint_16t> memory);
