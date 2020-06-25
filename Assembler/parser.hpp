#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "code.hpp"
#include "const.hpp"
#include "util.hpp"

using namespace std;

/**
 * Assemble the code on the path
 */
vector<u16> assembleCode(string path);