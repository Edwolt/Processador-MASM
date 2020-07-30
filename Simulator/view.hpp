#include <GL/glut.h>  // Linux

#include <iostream>
#include <utility>
#include <vector>

#include "View/led.hpp"
#include "View/video.hpp"

typedef void Loop();

struct View {
    Led led[10];
    Video video = Video(80, 45, 0, 0);

    View();

    void setup();
    void draw();
};