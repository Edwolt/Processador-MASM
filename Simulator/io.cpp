#include "io.hpp"

IO::IO(View* view) {
    this->view = view;
}

bool IO::in(u16 address, u16 src, u16 dest, u16& val, i32& delay) {
    if (address == 0) {  // halt
        delay = -1;
        return false;
    } else if (address == 7) {  // sleep
        delay = src;
        return false;
    }
    return false;
}

void IO::out(u16 address, u16 src, u16 dest) {
    if (address == 0) {
        for (int i = 0; i < LED_COUNT; i++) {
            view->led[i].on = src & (1 << i);
        }
    } else if (address == 3) {
        view->video.set(dest, src);
    }
}