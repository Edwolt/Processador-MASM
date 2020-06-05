#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OP_J     0xb0000
#define OP_CMP   0xb0001
#define OP_STORE 0xb0010
#define OP_LOAD  0xb0011
#define OP_INOUT 0xb0100
#define OP_SET   0xb0101
#define OP_MOVE  0xb0110
#define OP_ADD   0xb0111
#define OP_SUB   0xb1000
#define OP_MUL   0xb1001
#define OP_DIV   0xb1010
#define OP_SHIFT 0xb1011
#define OP_AND   0xb1100
#define OP_OR    0xb1101
#define OP_XOR   0xb1110
#define OP_NOT   0xb1111

#define TAM_MEM 65536

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Missing arguments\n");
        return EXIT_FAILURE;
    }

    int i, j;

    int memory[TAM_MEM];
    for (i = 0; i < TAM_MEM; i++) {
        memory[i] = 0;
    }

    FILE *source = fopen(argv[1], "r");
    FILE *dest = fopen(argv[2], "wb");

    char name[20];
    int instruction;

    i = 0;
    while (fscanf(source, " %s", name) == 1) {
        if (strcmp(name, "j")) {
            instruction = OP_J << 12;
        } else if (strcmp(name, "cmp") == 1) {
            instruction = OP_CMP << 12;
        } else if (strcmp(name, "store") == 1) {
            instruction = OP_STORE << 12;
        } else if (strcmp(name, "load") == 1) {
            instruction = OP_LOAD << 12;
        } else if (strcmp(name, "in") == 1) {
            instruction = OP_INOUT << 12;
        } else if (strcmp(name, "out") == 1) {
            instruction = OP_INOUT << 12;
        } else if (strcmp(name, "set") == 1) {
            instruction = OP_SET << 12;
        } else if (strcmp(name, "move") == 1) {
            instruction = OP_MOVE << 12;
        } else if (strcmp(name, "add") == 1) {
            instruction = OP_ADD << 12;
        } else if (strcmp(name, "sub") == 1) {
            instruction = OP_SUB << 12;
        } else if (strcmp(name, "mul") == 1) {
            instruction = OP_MUL << 12;
        } else if (strcmp(name, "div") == 1) {
            instruction = OP_DIV << 12;
        } else if (strcmp(name, "and") == 1) {
            instruction = OP_AND << 12;
        } else if (strcmp(name, "or") == 1) {
            instruction = OP_OR << 12;
        } else if (strcmp(name, "xor") == 1) {
            instruction = OP_XOR << 12;
        } else if (strcmp(name, "not") == 1) {
            instruction = OP_NOT << 12;
        }
        memory[i] = instruction;
        i++;
    }

    for (i = TAM_MEM; i > 0; i--) {
        if (memory[i] != 0) break;
    }

    fwrite(&i, sizeof(int), 1, dest);

    for (j = 0; j <= i; j++) {
        fwrite(memory + j, sizeof(int), 1, dest);
    }

    fclose(source);
    fclose(dest);
}