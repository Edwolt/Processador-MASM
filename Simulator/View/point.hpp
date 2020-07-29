#pragma once

using namespace std;

struct point {
   private:
    float x;
    float y;

   public:
    float getX() {
        return (x / (240 / 2)) - 1;
    }

    float getY() {
        return (-y / (140 / 2)) + 1;
    }

    void setX(float x) {
        this->x = x;
    }

    void setY(float y) {
        this->y = y;
    }

    point(float x, float y) {
        this->x = x;
        this->y = y;
    }

    point operator+(point &that) {
        return point(this->x + that.x, this->y + that.y);
    }
};
