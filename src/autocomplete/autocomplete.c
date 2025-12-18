#include "autocomplete.h"
#include "../builtins/builtins.h"
#include <dirent.h>
#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char **executables;
int capacity, size;

int sort(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void create_autocomplete_cache() {
    capacity = 2;
    size = 0;
    executables = malloc(sizeof(char *) * capacity);
    if (!executables)
        printf("malloc error");
    for (int i = 0; i < command_count; i += 1) {
        if (size >= capacity) {
            capacity += capacity;
            char **temp = realloc(executables, sizeof(char *) * capacity);
            if (temp) 
                executables = temp;
        }
        executables[size++] = strdup(commands[i].name);
        if (!executables[size - 1])
            printf("strdup error");
    }
    char *PATH = strdup(getenv("PATH"));
    if (PATH) {
        char *save_ptr = NULL;
        char *target_dir = strtok_r(PATH, PATH_LIST_SEPARATOR, &save_ptr);
        while (target_dir) {
            struct dirent *entry;
            DIR *dir = opendir(target_dir);
            if (dir == NULL) {
                continue;
            }
            while ((entry = readdir(dir)) != NULL) {
                char *full_path =
                    malloc(strlen(target_dir) + 2 + strlen(entry->d_name));
                if (full_path == NULL) {
                    continue;
                }
                strcpy(full_path, target_dir);
                strcat(full_path,
                       "/"); // Probably add windows compatibility
                             // for forward slash
                strcat(full_path, entry->d_name);
                if (access(full_path, X_OK) != -1) {
                    if (size >= capacity) {
                        capacity += capacity;
                        char **temp =
                            realloc(executables, sizeof(char *) * capacity);
                        if (temp)
                            executables = temp;
                    }
                    executables[size++] = strdup(entry->d_name);
                    if (!executables[size - 1])
                        printf("strdup error");
                }
                free(full_path);
            }
            closedir(dir);
            target_dir = strtok_r(NULL, PATH_LIST_SEPARATOR, &save_ptr);
        }
    } else {
        printf("No $PATH set");
    }
    qsort(executables, size, sizeof(char *), sort);
    int idx = 1;
    capacity = size;
    size = 1;
    for(int i = 1; i < capacity; i += 1) {
        if (strcmp(executables[i], executables[i - 1]) != 0) {
            executables[idx++] = executables[i];
        }
    }
    char **temp = realloc(executables, sizeof(char *) * idx + 1);
    executables = temp;
    size = idx;
}

char *command_generator(const char *text, int state) {
    static int list_index, len;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while (list_index < size) {
        char *name = executables[list_index];
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
