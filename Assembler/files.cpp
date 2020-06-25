#include "files.hpp"

void writeMIF(string path, vector<u16> memory) {
    ofstream file(path);
    file << "-- Codigo gerado pelo montador" << endl;
    file << "WIDTH=16;" << endl;
    file << "DEPTH=" << MEM_DEPTH << ";" << endl;
    file << "ADDRESS_RADIX=UNS" << endl;
    file << "DATA_RADIX=BIN;" << endl;
    file << "CONTENT BEGIN" << endl;

    for (unsigned i = 0; i < memory.size() && i < MEM_DEPTH; i++) {
        file << i << ':';
        for (int j = 15; j >= 0; j--) {
            file << ((memory[i] >> j) % 2);
        }
        file << ';' << endl;
    }

    if (memory.size() < MEM_DEPTH - 1) {
        file << memory.size() << ".." << (MEM_DEPTH - 1) << ":0000000000000000;" << endl;
    } else if (memory.size() < MEM_DEPTH) {
        file << (MEM_DEPTH - 1) << ":0000000000000000;" << endl;
    }
    file << "END;" << endl;

    file.close();
    cass << "Assembled to MIF: " << path << endl;
}

void writeBinary(string path, vector<u16> memory) {
    cass << "Assembled to Binary (not supported yet): " << path << endl;
}

const char* hexDigit = "0123456789ABCDEF";
inline static char printable(char c) { return (20 <= c && c <= 0x7F ? c : '.'); }
void writeText(string path, vector<u16> memory) {
    ofstream file(path);
    file.fill(' ');

    if (!file) {
        cerror << "NOT FILE" << endl;
        return;
    }
    for (u16 i : memory) {
        for (int j = 15; j >= 0; j--) {
            file << ((i >> j) % 2);
        }
        file << "        #";
        for (int j = 0; j < 4; j++) {
            file << hexDigit[(i >> j * 4) & 0x000F];
        }
        file << "    '" << printable((char)(i >> 8)) << printable((char)i) << "'    ";
        file.width(5);
        file << i;
        file << endl;
    }
    file << "----------------" << endl;

    file.close();
    cass << "Assembled to Text: " << path << endl;
}