#pragma once

using namespace std;

struct point {
    float x;
    float y;

    float calcX() {
        return (x / (80 / 2)) - 1;
    }

    float calcY() {
        return (-y / (52 / 2)) + 1;
    }

    point(float x, float y) {
        this->x = x;
        this->y = y;
    }

    point operator+(point &that) {
        return point(this->x + that.x, this->y + that.y);
    }

    void operator+=(point &that) {
        this->x += that.x;
        this->y += that.y;
    }
};
