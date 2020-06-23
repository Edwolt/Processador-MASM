#pragma once

#define MARK_ERROR   "\033[1;31m!!\033[0m "
#define MARK_ASERROR "\033[1;41m!!\033[0m "
#define MARK_WARNING "\033[1;35m##\033[0m "
#define MARK_DEBUG   "\033[1;32m??\033[0m "
#define MARK_DEBUGR  "\033[1;36m<<\033[0m "

#define LINE_ERROR   MARK_ERROR "\033[1m Line %d:\033[0m "
#define LINE_WARNING MARK_WARNING "\033[1m Line %d:\033[0m "