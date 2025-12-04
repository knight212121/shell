#ifndef INPUT_H
#define INPUT_H

#include "../utils/common.h"

CommandArgs* tokenize_input(const char* input);

void free_command_args(CommandArgs* cmd);

#endif
