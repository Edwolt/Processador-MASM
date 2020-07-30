#pragma once

#include <GL/glut.h>  // Linux

#include "../util.hpp"
#include "point.hpp"

struct square {
    point pos = point(0, 0);
    point size = point(1, 1);
    float r = 0.0, g = 0.0, b = 0.0;

    square(){};
    square(float x, float y);
    square(float x, float y, float r, float g, float b);

    void draw();
};

struct Video {
    int w = 0;
    int h = 0;
    point pos = point(0, 0);
    square **pixels = NULL;

    Video(){};
    Video(int w, int h, float x, float y);
    ~Video();

    void setup();
    void set(u16 pos, u16 color);
    void draw();
};