#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

History *history;

void initialize_history(int size) {
    history = malloc(sizeof(History *));
    history->max_size = size;
    history->begin = 0;
    history->lines = malloc(sizeof(char *) * size);
    for(int i = 0; i < size; i += 1)
        history->lines[i] = NULL;
}

void add_to_history(char *command) {
    if (history->lines[history->begin] != NULL)
        free(history->lines[history->begin]);
    history->lines[history->begin] = strdup(command);
    history->begin = (history->begin + 1) % history->max_size;
}

void print_history() {
    int i;
    int begin = history->begin;
    for(i = 0; i < history->max_size; i += 1) {
        if (history->lines[begin] != NULL)
            printf("%d: %s\n", i, history->lines[begin]);
        begin += 1;
        if (begin >= history->max_size)
            begin = 0;

    }
}
