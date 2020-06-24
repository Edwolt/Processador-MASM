#pragma once

#include <iostream>

extern bool debug;

#define cerror   cout << "\033[1;31m[ERROR]\033[0m "
#define cwarning cout << "\033[1;35m[WARN ]\033[0m "
#define cbug     cout << "\033[1;41m[ BUG ]\033[0m "
#define cdebug   if (debug) cout << "\033[1;32m[DEBUG]\033[0m "
#define cdebugr  if (debug) cout << "\033[1;36m[DEBUG]\033[0m "
#define cass     cout << "\033[1;33m>>\033[0m "

#define lerror(line)   cout << "\033[1;31m[ERROR] Line " << (line) << ":\033[0m "
#define lwarning(line) cout << "\033[1;35m[WARN ] Line " << (line) << ":\033[0m "