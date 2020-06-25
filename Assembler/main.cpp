// TODO reaname all types to turn its name more significative

#include <string.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "files.hpp"
#include "parser.hpp"
#include "util.hpp"

using namespace std;

/**
 * Print the help menssage
 */
void printHelp() {
    cout << "./assembler <source> [options]..." << endl;
    cout << "Options:" << endl;
    cout << "\t-m <file>\tAssemble and save as mif on <file>" << endl;
    cout << "\t-o <file>\tAssemble and save as binary on <file>" << endl;
    cout << "\t-1 <file>\tAssemble and save as a text with binary representation of result on <file>" << endl;
    cout << "\t-v <file>\tDebug" << endl;
    cout << "\t-h, --help\tDisplay this menssage" << endl;
}

int main(int argc, char const *argv[]) {
    // Extract arguments
    debug = false;
    string mifPath = "";
    string binaryPath = "";
    string textPath = "";
    string sourcePath = "";
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'm') {
                mifPath = string(argv[++i]);
            } else if (argv[i][1] == 'o') {
                binaryPath = string(argv[++i]);
            } else if (argv[i][1] == '1') {
                textPath = string(argv[++i]);
            } else if (argv[i][1] == 'h' || ((argv[i][1] == '-') && strcmp(argv[i], "--help") == 0)) {
                printHelp();
                return EXIT_SUCCESS;
            } else if (argv[i][1] == 'v') {
                debug = true;
            } else {
                cout << "Invalid flag" << endl;
            }
        } else {
            if (sourcePath.empty()) {
                sourcePath = argv[i];
            } else {
                cout << "Multiple souces are not supported" << endl;
                cout << "Using %s" << sourcePath << endl;
            }
        }
    }
    if (sourcePath.empty()) {
        printf("Missing source\n");
        return EXIT_FAILURE;
    }
    cout << (debug ? "Debug mode on\n" : "Debug mode off") << endl;

    // Parse code
    vector<u16> memory = assembleCode(sourcePath);

    // Calculates memory tam
    while (!memory.empty()) {
        if (memory.back() == 0) {
            memory.pop_back();
        } else {
            break;
        }
    }
    if (memory.empty()) {
        memory.push_back(0);
    }

    // Write compiled files
    cout << endl;
    if (!mifPath.empty()) writeMIF(mifPath, memory);
    if (!binaryPath.empty()) writeBinary(binaryPath, memory);
    if (!textPath.empty()) writeText(textPath, memory);
}