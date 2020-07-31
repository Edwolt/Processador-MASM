#pragma once

#include <GL/glut.h>  // Linux

#include <cmath>
#include <vector>

#include "point.hpp"

void ledSetup();

struct Led {
    point pos = point(0, 0);
    bool on = false;

    Led(){};
    Led(float x, float y);

    void draw();
};