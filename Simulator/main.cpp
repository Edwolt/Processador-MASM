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
bool playing = false;

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    view.draw();
    glutSwapBuffers();
}

void timer(int val) {
    if (playing) {
        for (int i = 0; i < 100; i++) processor.next();
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '+' && !playing) {
        processor.next();
    } else if (key == '-') {
        playing = !playing;
    } else {
        io.addChar(key);
    }
}

void setup(int argc, char** argv) {
    view.setup();

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(1600, 900);

    glutInitWindowPosition(0, 0);
    glutCreateWindow("Simulator");

    glClearColor(0.121, 0.415, 0.709, 1.0);

    glutDisplayFunc(draw);
}

void onExit() {
    cout << endl
         << "=======================================" << endl
         << "Cicles: " << processor.numClock << endl
         << "Instruction read: " << processor.numInstructions << endl
         << "Instructions that isn't noop: " << processor.numExecuted << endl
         << "jumps read: " << processor.numJumps << endl
         << "Jumps executed: " << processor.numJumpsExecuted << endl
         << "Memory access: " << processor.numMemory << endl
         << "Memory random access: " << processor.numMemoryRandom << endl
         << "Input and output: " << processor.numIO << endl;
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

    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    // glutFullScreen();
    atexit(onExit);
    glutMainLoop();

    return EXIT_SUCCESS;
}
