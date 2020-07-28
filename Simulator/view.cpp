#include <GL/glut.h>  // Linux

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

struct point {
   private:
    float x;
    float y;

   public:
    float getX() {
        return x / 200;
    }

    float getY() {
        return y / 100;
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
};

vector<point> ledShape;
struct Led {
    point pos = point(0, 0);

    Led(float x, float y) {
        this->pos = point(x, y);
    }
};

struct Seg {
};

struct Switch {
};

struct Button {
};

struct Video {
};

void setup() {
    for (float i = 0; i < 360; i += 5) {
        float x = 1 * cos(i / 180 * M_PI);
        float y = 1 * sin(i / 180 * M_PI);
        ledShape.push_back(point(x, y));
    }
}
void timer(int dt) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 0, 0);
    glBegin(GL_POLYGON);
    for (point i : ledShape) {
        glVertex2d(i.getX(), i.getY());
    }
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char const *argv[]) {
    setup();

    glutInit(&argc, (char **)argv);

    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(1200, 600);

    glutInitWindowPosition(0, 0);
    glutCreateWindow("Simulator");

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(draw);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    cout << "oi\n";
}
