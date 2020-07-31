#pragma once

#include <GL/glut.h>  // Linux

#include <iostream>
#include <utility>
#include <vector>

#include "View/led.hpp"
#include "View/video.hpp"

typedef void Loop();

#define LED_COUNT 16

struct View {
    Led led[LED_COUNT];
    Video video = Video(80, 45, 0, 0);

    View();

    void setup();
    void draw();
};