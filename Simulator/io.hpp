#pragma once

#include <iostream>

#include "util.hpp"
#include "view.hpp"

using namespace std;

struct IO {
    View* view = NULL;

    IO() {}
    IO(View* view);

    bool in(u16 address, u16 src, u16 dest, u16& val, i32& delay);
    void out(u16 address, u16 src, u16 dest);
};