#ifndef INPUT_H
#define INPUT_H

#include "../utils/common.h"

Sequence* tokenize_input(char* input);

void free_command_args(Sequence* pipes);

#endif
