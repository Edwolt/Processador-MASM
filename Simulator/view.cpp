#include "view.hpp"

#include <GL/glut.h>  // Linux

using namespace std;

struct Seg {
};

struct Switch {
};

struct Button {
};

View::View() {
}

void View::setup() {
    for (int i = 0; i < 10; i++) led[i] = Led(1 + 5 * i, 52 - 5);

    ledSetup();
    video.setup();
}

void View::draw() {
    video.draw();
    for (int i = 0; i < 10; i++) led[i].draw();
}