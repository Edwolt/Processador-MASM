#include "video.hpp"

//* ================== *//
//* ===== square ===== *//
//* ================== *//

square::square(float x, float y) {
    this->pos = point(x, y);
}

square::square(float x, float y, float r, float g, float b) {
    this->pos = point(x, y);
    this->r = r;
    this->g = g;
    this->b = b;
}

void square::draw() {
    glColor3f(r, g, b);

    glBegin(GL_POLYGON);

    point aux = this->pos;

    glVertex2d(pos.calcX(), pos.calcY());

    aux.y += 1;
    glVertex2d(aux.calcX(), aux.calcY());

    aux.x += 1;
    glVertex2d(aux.calcX(), aux.calcY());

    aux.y -= 1;
    glVertex2d(aux.calcX(), aux.calcY());

    glEnd();
}

//* ================= *//
//* ===== Video ===== *//
//* ================= *//

#include <iostream>
Video::Video(int w, int h, float x, float y) {
    pos = point(x, y);
    this->w = w;
    this->h = h;

    pixels = new square*[w];
    for (int i = 0; i < w; i++) pixels[i] = new square[h];
}

Video::~Video() {
    if (pixels) {
        for (int i = 0; i < w; i++) delete[] pixels[i];
        delete[] pixels;
    }
    pixels = NULL;
}

void Video::setup() {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            pixels[i][j] = square(i + pos.x, j + pos.y, 0, 0, 0);
        }
    }
}

void Video::draw() {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            pixels[i][j].draw();
        }
    }
}

void Video::set(u16 pos, u16 color) {

    pos %= w * h;
    int i = pos % w;
    int j = pos / w;
    float r = (color >> 12) / 16.0;
    float g = ((color >> 8) & 0x000F) / 16.0;
    float b = ((color >> 4) & 0x000F) / 16.0;

    pixels[i][j].r = r;
    pixels[i][j].g = g;
    pixels[i][j].b = b;
}