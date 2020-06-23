#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "message.hpp"

using namespace std;

const int MEM_DEPTH = 2 << 16;

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
    int i, j;

    // Extract arguments
    debug = false;
    string mifPath = "";
    string binaryPath = "";
    string textPath = "";
    string sourcePath = "";
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'm') {
                mifPath = string(argv[++i]);
            } else if (argv[i][1] == 'o') {
                binaryPath = string(argv[++i]);
            } else if (argv[i][1] == '1') {
                textPath = string(argv[++i]);
            } else if (argv[i][1] == 'h' || ((argv[i][1] == '-') && strcmp(argv[i], "--help") == 0)) {
                printHelp();
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
    vector<uint16_t> memory = codeParse();

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
    ofstream dest;
    if (!mifPath.empty()) {
        dest.open(mifPath, ios::out | ios::trunc);

        dest << "-- Codigo gerado pelo montador" << endl;
        dest << "WIDTH=16;" << endl;
        dest << "DEPTH=" << MEM_DEPTH << ";" << endl;
        dest << "ADDRESS_RADIX=UNS" << endl;
        dest << "DATA_RADIX=BIN;" << endl;
        dest << "CONTENT BEGIN" << endl;
        for (i = 0; i < memTam; i++) {
            dest << i << ':';
            for (j = 15; j >= 0; j--) {
                dest << ((memory[i] >> j) % 2);
            }
            dest << ';' << endl;
        }
        if (i < MEM_DEPTH - 1) {
            dest << i << ".." << (MEM_DEPTH - 1) << ':0000000000000000;' << endl;
        } else if (i < MEM_DEPTH) {
            dest << (MEM_DEPTH - 1) << ':0000000000000000;' << endl;
        }
        dest << "END;" << endl;

        dest.close();
        cass << "Assembled to MIF: " << mifPath << endl;
    }
    if (!binaryPath.empty()) {
        cass << "Assemble to Binary not supported yet: " << binaryPath << endl;
    }
    if (textPath) {
        dest = fopen(textPath, "w");

        for (i = 0; i < memTam; i++) {
            for (j = 15; j >= 0; j--) {
                fprintf(dest, "%d", (memory[i] >> j) % 2);
            }
            fprintf(dest, "   #%04X   %d\n", memory[i], memory[i]);
        }
        fprintf(dest, "----------------\n");

        fclose(dest);
        messageOut("Assembled to Text: %s\n", textPath);
    }

    free(memory);
}
