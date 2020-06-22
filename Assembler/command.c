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

Command* commandValue(uint16_t value) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_VALUE;
    command->value = value;
    return command;
}

Command* commandList(uint16_t len, uint16_t* list) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_LIST;
    command->value = len;
    command->list = list;
    return command;
}

Command* commandSpace(uint16_t len) {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_SPACE;
    command->value = len;
    return command;
}

Command* commandEnd() {
    Command* command = malloc(sizeof(Command));
    if (!command) return NULL;

    command->type = COMMAND_END;
    return command;
}

void deleteCommand(Command* command) {
    if (!command) return;

    if (command->type == COMMAND_LABEL && command->label) {
        free(command->label);
    } else if (command->type == COMMAND_LIST && command->list) {
        free(command->list);
    }

    free(command);
}
