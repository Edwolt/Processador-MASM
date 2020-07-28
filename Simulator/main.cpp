// #include <GL/glut.h>  // Linux

#include <iostream>
#include <string>
#include <vector>

#include "files.hpp"
#include "util.hpp"

using namespace std;

const char* hexDigit = "0123456789ABCDEF";
int main(int argc, char const* argv[]) {
    if (argc < 2) {
        cout << "Missing source" << endl;
        return EXIT_FAILURE;
    }

    string path(argv[1]);
    vector<u16> memory = readBinary(path);

    for (u16 i : memory) {
        if (i != 0) {
            for (int j = 4 - 1; j >= 0; j--) {
                cout << hexDigit[(i >> j * 4) & 0x000F];
            }
            cout << endl;
        }
    }
    return 0;
}