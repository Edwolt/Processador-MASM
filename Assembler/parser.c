#include "parser.h"

enum State {
    END_ARGUMENT = 0,
    EXPECT_REGISTER1 = 1,
    EXPECT_REGISTER2 = 2,
    EXPECT_REGISTER3 = 3,
    EXPECT_VALUE,
    NEXT_CHUNCK,
    EXPECT_NEWLINE,
    END_FILE,
};

struct _Parser {
    FILE* file;
    enum State state;
    int line;
};

#define TAM_COMMAND 25

Parser* createParser(char* path) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;

    parser->file = fopen(path, "r");
    if (!parser->file) {
        free(parser);
        return NULL;
    }

    parser->state = NEXT_CHUNCK;
    parser->line = 1;
}

void deleteParser(Parser* parser) {
    if (!parser) return;
    fclose(parser->file);
    free(parser);
}

void deleteCommand(Command* command) {
    if (!command) return;
    if (command->what == COMMAND_LABEL && command->label) {
        free(command->label);
    }
    free(command);
}

Command* parser_nextChunck(Parser* parser) {
    if (!parser) return;

    int i;
    char c;

    Command* command = malloc(sizeof(command));
    command->label = malloc(TAM_COMMAND * sizeof(char));

    for (i = 0; i < TAM_COMMAND; i++) {
        fscanf("%c", &c);
        if (c == ':') {
            command->what = COMMAND_LABEL;
            command->label[i] = '\0';
            return command;
        }
        if (c == ' ') {
        }
    }
}