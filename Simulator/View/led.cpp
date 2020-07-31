#include "led.hpp"

static vector<point> ledShape;

void ledSetup() {
    ledShape.push_back(point(2, 2));
    for (float i = 0; i < 180; i += 1) {
        float x = 1 * cos(i / 180 * M_PI);
        float y = 1 * sin(-i / 180 * M_PI);
        ledShape.push_back(point(x + 1, y + 1));
    }
    ledShape.push_back(point(0, 2));
}

Led::Led(float x, float y) {
    this->pos = point(x, y);
}

void Led::draw() {
    if (this->on) {
        glColor3f(1.0, 0, 0);
    } else {
        glColor3f(0.3, 0, 0);
    }

    glBegin(GL_POLYGON);
    for (point i : ledShape) {
        i = i + pos;
        glVertex2d(i.calcX(), i.calcY());
    }
    glEnd();
}