#include "memory.h"

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

#define TAM_COMMAND

int memory_create(int* mem, char* path, int* n) {
    int i = 0;
    int instruction;
    char command[TAM_COMMAND];
    while (fscanf(command, " %s", command) == 1) {
        if (strcmp(command, "j")) {
            instruction = OP_J << 12;
        } else if (strcmp(command, "cmp") == 1) {
            instruction = OP_CMP << 12;
        } else if (strcmp(command, "store") == 1) {
            instruction = OP_STORE << 12;
        } else if (strcmp(command, "load") == 1) {
            instruction = OP_LOAD << 12;
        } else if (strcmp(command, "in") == 1) {
            instruction = OP_INOUT << 12;
        } else if (strcmp(command, "out") == 1) {
            instruction = OP_INOUT << 12;
        } else if (strcmp(command, "set") == 1) {
            instruction = OP_SET << 12;
        } else if (strcmp(command, "move") == 1) {
            instruction = OP_MOVE << 12;
        } else if (strcmp(command, "add") == 1) {
            instruction = OP_ADD << 12;
        } else if (strcmp(command, "sub") == 1) {
            instruction = OP_SUB << 12;
        } else if (strcmp(command, "mul") == 1) {
            instruction = OP_MUL << 12;
        } else if (strcmp(command, "div") == 1) {
            instruction = OP_DIV << 12;
        } else if (strcmp(command, "and") == 1) {
            instruction = OP_AND << 12;
        } else if (strcmp(command, "or") == 1) {
            instruction = OP_OR << 12;
        } else if (strcmp(command, "xor") == 1) {
            instruction = OP_XOR << 12;
        } else if (strcmp(command, "not") == 1) {
            instruction = OP_NOT << 12;
        }
        mem[i] = instruction;
        i++;
    }
}