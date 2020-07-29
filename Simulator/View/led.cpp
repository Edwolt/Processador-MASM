#include "led.hpp"

static vector<point> ledShape;

void ledSetup() {
    for (float i = 0; i < 360; i += 5) {
        float x = 1 * cos(i / 180 * M_PI);
        float y = 1 * sin(i / 180 * M_PI);
        ledShape.push_back(point(x * 2 + 2, y * 2 + 2));
    }
}

Led::Led(float x, float y) {
    this->pos = point(x, y);
}

void Led::draw() {
    glColor3f(1.0, 0, 0);

    glBegin(GL_POLYGON);
    for (point i : ledShape) {
        // cout << 'o';
        i = i + pos;
        glVertex2d(i.getX(), i.getY());
    }
    // cout << endl;
    glEnd();
}