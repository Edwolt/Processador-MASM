// #include <GL/glut.h>  // Linux

#include <iostream>
#include <string>
#include <vector>

#include "files.hpp"
#include "util.hpp"

using namespace std;

const int MEM_DEPTH = 1 << 16;

int main(int argc, char const *argv[]) {
    string path = "";
    vector<u16> memory = readBinary(path);

    for (u16 i : memory) {
        cout << hex << i << endl;
    }
    return 0;
}