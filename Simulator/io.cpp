#include "io.hpp"

IO::IO(View* view) {
    this->view = view;
}

bool IO::in(u16 address, u16 val1, u16 val2, u16& val, i32& delay) {
    if (address == 0) {  // halt
        delay = -1;
        return false;
    } else if (address == 3) {
        val = this->c;
        this->c = 0;
        return true;
    } else if (address == 7) {  // sleep
        delay = val1;
        return false;
    }
    return false;
}

void IO::out(u16 address, u16 val1, u16 val2) {
    if (address == 0) {
        for (int i = 0; i < LED_COUNT; i++) {
            view->led[i].on = val1 & (1 << i);
        }
    } else if (address == 3) {
        view->video.set(val1, val2);
    }
}

void IO::addChar(unsigned char c) {
    this->c = (u16)c;
}