#include <GL/glut.h>  // Linux

#include <iostream>
#include <utility>
#include <vector>

#include "View/led.hpp"

typedef void Loop();

struct View {
    Led led;

    View();
    ~View();
    void setup();

    void draw();
};