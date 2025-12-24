#ifndef HISTORY_H
#define HISTORY_H

typedef struct {
    char **lines;
    int max_size;
    int begin;
    int insertion_count;
} History;

void initialize_history(int size);
void add_to_history(char* command);
void print_history(int offset);
void free_history();

#endif
