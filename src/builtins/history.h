#ifndef HISTORY_H
#define HISTORY_H

typedef struct {
    char **lines;
    int max_size;
    int begin;
} History;

void initialize_history(int size);
void add_to_history(char* command);
void print_history();
void free_history();

#endif
