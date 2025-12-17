#include "autocomplete.h"
#include "../builtins/builtins.h"
#include <stdio.h>
#include <readline/readline.h>
#include <string.h>

char *command_generator(const char *text, int state) {
    static int list_index, len;

    if(!state) {
        list_index = 0;
        len = strlen(text);
    }

    while (list_index < command_count) {
        char *name = commands[list_index].name;
        list_index++;

        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

char **input_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    if (start == 0)
        return rl_completion_matches(text, command_generator);
    return NULL;
}
