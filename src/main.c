#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "utils/common.h"
#include "input/input.h"
#include "autocomplete/autocomplete.h"
#include "executor/executor.h"

int main(int argc, char *argv[]) {
    create_autocomplete_cache();
    rl_attempted_completion_function = input_completion;
    while (1) {
        setbuf(stdout, NULL);

        char* input = readline("$ ");

        CommandArgs* cmd = tokenize_input(input);

        execute_command(cmd);

        free_command_args(cmd);

        free(input);
    }

    return 0;
}
