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
    history->insertion_count = 0;
}

void add_to_history(char *command) {
    if (history->lines[history->begin] != NULL)
        free(history->lines[history->begin]);
    history->lines[history->begin] = strdup(command);
    history->begin = (history->begin + 1) % history->max_size;
    history->insertion_count += 1;
}

void print_history() {
    int total_entries, begin;
    if (history->max_size < history->insertion_count) {
        total_entries = history->max_size;
        begin = history->begin % history->max_size;
    } else {
        total_entries = history->insertion_count;
        begin = 0;
    }

    for(int i = 0; i < total_entries; i += 1) {
        if(history->lines[begin] != NULL)
            printf("    %d  %s\n", i + 1, history->lines[begin]);
        begin = (begin + 1) % history->max_size;
    }
}
