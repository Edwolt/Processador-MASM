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
    for (int i = 0; i < LED_COUNT; i++) led[i] = Led(1 + 3 * i, 52 - 3);

    ledSetup();
    video.setup();
}

void View::draw() {
    video.draw();
    for (int i = 0; i < LED_COUNT; i++) led[i].draw();

    point p(0, 45.3);
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_POLYGON);
    glVertex2d(p.calcX(), p.calcY());
    p.x = 80;
    glVertex2d(p.calcX(), p.calcY());
    p.y = 45.7;
    glVertex2d(p.calcX(), p.calcY());
    p.x = 0;
    glVertex2d(p.calcX(), p.calcY());
    glEnd();
}