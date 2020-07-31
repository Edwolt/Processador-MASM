#include "led.hpp"

static vector<point> ledShape;

void ledSetup() {
    for (float i = 0; i < 360; i += 20) {
        float x = 1 * cos(i / 180 * M_PI);
        float y = 1 * sin(i / 180 * M_PI);
        ledShape.push_back(point(x * 2 + 2, y * 2 + 2));
    }
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