#ifndef INPUT_H
#define INPUT_H

#include "../utils/common.h"

Pipeline* tokenize_input(char* input);

void free_command_args(Pipeline* pipes);

#endif
