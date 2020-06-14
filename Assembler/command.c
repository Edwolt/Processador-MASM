#include "command.h"

Command* commandNothing() {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_NOTHING;
    return command;
}

Command* commandLabel(char* label) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_LABEL;
    command->label = label;
    return command;
}

Command* commandCode(uint16_t value) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_CODE;
    command->value = value;
    return command;
}

Command* commandConst(uint16_t value) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_CONST;
    command->value = value;
    return command;
}

Command* commandNextLine(uint16_t value, uint16_t nextLine) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_NEXTLINE;
    command->value = value;
    command->nextLine = nextLine;
    return command;
}

Command* commandEndFile() {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_ENDFILE;
    return command;
}

void deleteCommand(Command* command) {
    if (!command) return;
    if (command->type == COMMAND_LABEL && command->label) {
        free(command->label);
    }
    free(command);
}
