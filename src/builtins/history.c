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
    history->insertion_count = 0;
}

void add_to_history(char *command) {
    if (history->begin >= history->max_size) {
        history->max_size *= 2;
        char **temp = realloc(history->lines, sizeof(char *) * history->max_size);
        if (!temp)
            return;
        history->lines = temp;
    }
    history->lines[history->begin++] = strdup(command);
    history->insertion_count += 1;
}

void print_history(int offset) {
    int i;
    if (offset == -1)
        i = 0;
    else
        i = history->insertion_count - offset;
    while(i < history->insertion_count) {
        if(history->lines[i] != NULL)
            printf("    %d  %s\n", i + 1, history->lines[i]);
        i += 1;
    }
}
