#include "history.h"
#include <stdio.h>
#include <readline/history.h>
#include <stdio.h>
#include <string.h>

void print_history(int offset) {
    if (offset == -1) {
        HIST_ENTRY **hist = history_list();
        for (int i = 0; hist[i] != NULL; i += 1) {
            printf("    %d  %s\n", i + 1, hist[i]->line);
        }
    } else {
        for (int i = 0; i < offset; i += 1) {
            HIST_ENTRY *entry = history_get(where_history() - offset + 2 + i);
            if (entry)
                printf("    %d  %s\n", where_history() - offset + i + 2, entry->line);
        }
    }
}
