#include "autocomplete/autocomplete.h"
#include "executor/executor.h"
#include "input/input.h"
#include "utils/common.h"
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static volatile int keep_running = 1;

void int_handler(int dummy) { keep_running = 0; }

char *history_file = NULL;

int main(int argc, char *argv[]) {
    create_autocomplete_cache();
    rl_attempted_completion_function = input_completion;
    using_history();
    stifle_history(1000);
    if (getenv("HISTFILE") != NULL)
        history_file = strdup(getenv("HISTFILE"));
    else
        history_file = strdup("/tmp/history");
    read_history(history_file);
    signal(SIGINT, int_handler);
    while (keep_running) {
        setbuf(stdout, NULL);

        char *input = readline("$ ");

        if (!input)
            break;

        if (*input)
            add_history(input);

        Sequence *sequence = tokenize_input(input);

        if (sequence != NULL) {
            for (int i = 0; i < sequence->count; i += 1) {
                Pipeline *pipe = sequence->pipelines[i];
                execute_command(pipe);
            }
        }

        free_command_args(sequence);
        free(input);
    }
    write_history(history_file);
    exit(0);
}
