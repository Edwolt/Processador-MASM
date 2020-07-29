#include "view.hpp"

#include <GL/glut.h>  // Linux

using namespace std;

struct Seg {
};

struct Switch {
};

struct Button {
};

struct Video {
};

View::View() {
    ledSetup();

    led = Led(5, 136);
}
View::~View() {
}

void View::setup() {
    ledSetup();

    led = Led(5, 136);
}

void View::draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    led.draw();

    glutSwapBuffers();
}