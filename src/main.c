#include "autocomplete/autocomplete.h"
#include "builtins/history.h"
#include "executor/executor.h"
#include "input/input.h"
#include "utils/common.h"
#include <stdio.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    create_autocomplete_cache();
    rl_attempted_completion_function = input_completion;
    initialize_history(1);
    while (1) {
        setbuf(stdout, NULL);

        char *input = readline("$ ");

        Pipeline *pipes = tokenize_input(input);

        if (pipes != NULL) {
            add_to_history(input);

            execute_command(pipes);
        }
        free_command_args(pipes);

        free(input);
    }

    return 0;
}
