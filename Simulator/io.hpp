#pragma once

#include <iostream>

#include "util.hpp"
#include "view.hpp"

using namespace std;

struct IO {
    View* view = NULL;
    u16 c = 0;

    IO() {}
    IO(View* view);

    bool in(u16 address, u16 val1, u16 val2, u16& val, i32& delay);
    void out(u16 address, u16 val1, u16 val2);

    void addChar(unsigned char c);
};