#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "stdarg.h"

bool debug;

#define MARK_ERROR   "\033[1;31m!!\033[0m "
#define MARK_WARNING "\033[1;35m##\033[0m "
#define MARK_BUG     "\033[1;41m!!\033[0m "
#define MARK_DEBUG   "\033[1;32m??\033[0m "
#define MARK_DEBUGR  "\033[1;36m<<\033[0m "
#define MARK_OUT     "\033[1;33m>>\033[0m "

#define LINE_ERROR   MARK_ERROR "\033[1mLine %d:\033[0m "
#define LINE_WARNING MARK_WARNING "\033[1mLine %d:\033[0m "

// Wrapper to printfs to make them beutyful
#define messageError(...)  printf(MARK_ERROR __VA_ARGS__)
#define messageWaning(...) printf(MARK_WARNING __VA_ARGS__)
#define messageBug(...)    printf(MARK_BUG __VA_ARGS__)
#define messageOut(...)    printf(MARK_OUT __VA_ARGS__)

#define parsingError(parser, ...)       \
    printf(LINE_ERROR, (parser)->line); \
    printf(__VA_ARGS__)

#define parsingWarning(parser, ...)       \
    printf(LINE_WARNING, (parser)->line); \
    printf(__VA_ARGS__)

#define messageDebug(...) \
    if (debug) printf(MARK_DEBUG __VA_ARGS__)

#define messageDebugr(...) \
    if (debug) printf(MARK_DEBUGR __VA_ARGS__)
