#pragma once

#include <iostream>

extern bool debug;

#define cerror   cout << "\033[1;31m[Erro]\033[0m "
#define cwarning cout << "\033[1;35m[Warn]\033[0m "
#define cbug     cout << "\033[1;41m!!\033[0m "
#define cdebug   if (debug) cout << "\033[1;32m??\033[0m "
#define cdebugr  if (debug) cout << "\033[1;36m<<\033[0m "
#define cass     cout << "\033[1;33m>>\033[0m "

#define lerror(line)   cerror << "\033[1mLine " << (line) << ":\033[0m "
#define lwarning(line) cwarning << "\033[1mLine " << (line) << ":\033[0m "