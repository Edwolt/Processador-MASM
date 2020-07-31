#include "files.hpp"

inline static u16 strToNum(char str[2]) {
    unsigned char *aux = (unsigned char *)str;
    return (aux[0] << 8) | aux[1];
}

vector<u16> readBinary(string path) {
    vector<u16> memory(MEM_DEPTH, 0);

    fstream file(path, ios::in | ios::binary);
    if (!file) {
        cout << "Can't open the file: " << path << endl;
        return memory;
    }

    char aux[2];
    file.read(aux, 2);

    u16 n = strToNum(aux);

    for (u16 i = 0; i < n; i++) {
        file.read(aux, 2);
        memory[i] = strToNum(aux);
    }

    return memory;
}