#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "parser.h"

/**
 * Create a array with the assembled code
 */
uint16_t *createMemory(char *path) {
    int i, j;

    Parser *parser = createParser(path);
    Command *command;

    uint16_t *memory = malloc(TAM_MEM * sizeof(uint16_t));
    if (!memory) return NULL;

    for (i = 0; i < TAM_MEM; i++) memory[i] = 0;

    i = 0;
    while (true) {
        command = parseNext(parser);
        if (!command) {
            messageBug("Assembler failed (parser returned NULL)\n");
            break;
        }

        switch (command->type) {
            case COMMAND_NOTHING:
                messageDebugr("Nothing\n");
                break;

            case COMMAND_LABEL:  // TODO
                messageDebugr("Label %s\n", command->label);

                break;

            case COMMAND_VALUE:
                messageDebugr("Value: %d\n", command->val);
                memory[i++] = command->val;
                break;

            case COMMAND_LIST:
                messageDebugr("List: [%d]\n", command->len);
                for (j = 0; j < command->len; i++) {
                    memory[i++] = command->list[j];
                }
                break;

            case COMMAND_SPACE:
                messageDebugr("Space: [%d, %d]\n", command->len, command->val);
                if (command->val == 0) {
                    i += command->len;
                    break;
                }

                for (j = 0; j < command->len; i++) {
                    memory[i++] = command->val;
                }
                break;

            case COMMAND_END:
                messageDebugr("End\n");
                goto while_break;

            default:
                messageBug("createMemory\n");
                return NULL;
        }
        deleteCommand(command);
    }
while_break:

    printf("\n");
    return memory;
}

// -m MIF file
// -o binary file
// -1 text file with 0s and 1s of all memory
int main(int argc, /*const*/ char *argv[]) {
    int i, j;

    // Extract arguments
    debug = false;
    char *mifPath = NULL;
    char *binaryPath = NULL;
    char *textPath = NULL;
    char *sourcePath = NULL;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'm') {
                mifPath = argv[++i];
            } else if (argv[i][1] == 'o') {
                binaryPath = argv[++i];
            } else if (argv[i][1] == '1') {
                textPath = argv[++i];
            } else if (argv[i][1] == 'h' ||
                       ((argv[i][1] == '-') && strcmp(argv[i], "--help") == 0)) {
                printf("./assembler <source> [options]...\n");
                printf("Options:\n");
                printf("\t-m <file>\tAssemble and save as mif on <file>\n");
                printf("\t-o <file>\tAssemble and save as binary on <file>\n");
                printf("\t-1 <file>\tAssemble and save as a text with binary representation of result on <file>\n");
                printf("\t-v <file>\tDebug\n");
                printf("\t-h, --help\tDisplay this menssage\n");
            } else if (argv[i][1] == 'v') {
                debug = true;
            } else {
                printf("Invalid flag\n");
            }
        } else {
            if (!sourcePath) {
                sourcePath = argv[i];
            } else {
                printf("Multiple souces are not supported\n");
                printf("Using %s\n", sourcePath);
            }
        }
    }
    if (!sourcePath) {
        printf("Missing source\n");
        return EXIT_FAILURE;
    }
    printf(debug ? "Debug mode on\n" : "Debug mode off\n");

    // Parse code
    uint16_t *memory = createMemory(sourcePath);
    if (!memory) {
        printf("Assemble failed\n");
    }

    // Calculates memory tam
    int memTam;
    for (memTam = TAM_MEM; memTam > 0; memTam--) {
        if (memory[memTam - 1] != 0) {
            break;
        }
    }

    // Write compiled files
    FILE *dest;
    if (mifPath) {
        dest = fopen(mifPath, "w");

        fprintf(dest, "-- Codigo gerado pelo montador\n");
        fprintf(dest, "WIDTH=16;\n");
        fprintf(dest, "DEPTH=%d;\n", TAM_MEM);
        fprintf(dest, "ADDRESS_RADIX=UNS\n");
        fprintf(dest, "DATA_RADIX=BIN;\n");
        fprintf(dest, "CONTENT BEGIN\n");
        for (i = 0; i < memTam; i++) {
            fprintf(dest, "%d:", i);
            for (j = 15; j >= 0; j--) {
                fprintf(dest, "%d", (memory[i] >> j) % 2);
            }
            fprintf(dest, ";\n");
        }
        if (i < TAM_MEM - 1) {
            fprintf(dest, "[%d..%d]:0000000000000000;\n", i, TAM_MEM - 1);
        } else if (i < TAM_MEM) {
            fprintf(dest, "%d:0000000000000000;\n", TAM_MEM - 1);
        }
        fprintf(dest, "END;\n");

        fclose(dest);
        messageOut("Assembled to MIF: %s\n", mifPath);
    }
    if (binaryPath) {
        messageOut("Assemble to Binary not supported yet: %s\n", binaryPath);
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