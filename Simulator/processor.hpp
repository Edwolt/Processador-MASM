#pragma once

#include <string>
#include <vector>

#include "files.hpp"
#include "util.hpp"

using namespace std;

struct Processor;

Processor* createProcessor(string path);
void next(Processor* processor);
bool hasNext(Processor* processor);

unsigned long long numInstructions(Processor* processor);
unsigned long long numExecuted(Processor* processor);
unsigned long long numJumps(Processor* processor);
unsigned long long numJumpsExecuted(Processor* processor);