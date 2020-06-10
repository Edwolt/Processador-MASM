#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

// -m MIF file
// -b binary file
// -0 text file with 0s and 1s of all memory
int main(int argc, char const *argv[]) {
    if (argc < 3) {
        printf("Missing arguments\n");
        return EXIT_FAILURE;
    }

    int i, j;

    uint16_t memory[TAM_MEM];
    for (i = 0; i < TAM_MEM; i++) {
        memory[i] = 0;
    }

    FILE *dest = fopen(argv[2], "wb");

    // TODO parse the assembly code

    for (i = TAM_MEM; i > 0; i--) {
        if (memory[i] != 0) break;
    }

    fwrite(&i, sizeof(int), 1, dest);

    for (j = 0; j <= i; j++) {
        fwrite(memory + j, sizeof(int), 1, dest);
    }

    fclose(dest);
}