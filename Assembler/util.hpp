#pragma once

extern bool debug;

#include <cstdint>

const int MEM_DEPTH = 1 << 16;

#define cerror   cout << "\033[1;31m!!\033[0m "
#define cwarning cout << "\033[1;35m##\033[0m "
#define cbug     cout << "\033[1;41m:(\033[0m "
#define cdebug   if (debug) cout << "\033[1;32m??\033[0m "
#define cdebugr  if (debug) cout << "\033[1;36m<<\033[0m "
#define cass     cout << "\033[1;33m>>\033[0m "

#define lerror(line)   cout << "\033[1;31m!! Line " << (line) << ":\033[0m "
#define lwarning(line) cout << "\033[1;35m## Line " << (line) << ":\033[0m "
#define ldebug(line)   if(debug) cout << "\033[1;32m?? Line " << (line) << ":\033[0m "

typedef uint16_t u16;
typedef uint32_t u32;

inline bool isSpace(char c) { return c == '\t' || c == '\v' || c == ' '; }
inline bool isEnter(char c) { return c == '\n' || c == '\f' || c == '\r'; }