#include "files.hpp"

void writeMIF(string path, vector<u16> memory) {
    ofstream file(path);
    file << "-- Codigo gerado pelo montador" << endl;
    file << "WIDTH=16;" << endl;
    file << "DEPTH=" << MEM_DEPTH << ";" << endl;
    file << "ADDRESS_RADIX=UNS" << endl;
    file << "DATA_RADIX=BIN;" << endl;
    file << "CONTENT BEGIN" << endl;

    for (unsigned i = 0; i < memory.size(); i++) {
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
    cass << "Assemble to Binary not supported yet: " << path << endl;
}

void writeText(string path, vector<u16> memory) {
    ofstream file(path);
    for (u16 i : memory) {
        for (int j = 15; j >= 0; j--) {
            file << ((memory[i] >> j) % 2);
        }
        file << "\t#" << hex << memory[i] << dec << memory[i] << endl;
    }
    file << "----------------" << endl;

    file.close();
    cass << "Assembled to Text: " << path << endl;
}