#include <GL/glut.h>  // Linux

#include <iostream>
#include <string>

#include "processor.hpp"
#include "util.hpp"
#include "view.hpp"

using namespace std;

static View view = View();
static Processor processor;
static IO io;

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    view.draw();
    glutSwapBuffers();
}

void timer(int val) {
    for (int i = 0; i < 100; i++) processor.next();

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, val + 100);
}

void setup(int argc, char** argv) {
    view.setup();

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(1600, 900);

    glutInitWindowPosition(0, 0);
    glutCreateWindow("Simulator");

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(draw);
}

void onExit() {
    cout << "Number of instruction read: " << processor.numInstructions << endl
         << "Number of instructions that isn't noop: " << processor.numExecuted << endl
         << "Number of jumps read: " << processor.numJumps << endl
         << "Number of jumps executed: " << processor.numJumpsExecuted << endl;
}

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        cout << "Missing source" << endl;
        return EXIT_FAILURE;
    }

    setup(argc, (char**)argv);

    string path(argv[1]);
    io = IO(&view);
    processor = Processor(path, &io);

    glutTimerFunc(0, timer, 0);
    glutFullScreen();
    atexit(onExit);
    glutMainLoop();

    return EXIT_SUCCESS;
}
