#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim_whitespace(char *str) {
    while (*str == ' ')
        str++;

    if (*str == 0)
        return str;

    char *end = str + strlen(str) - 1;

    while (end > str && *end == ' ') {
        *end = 0;
        end--;
    }

    return str;
}

void make_tokens(CommandArgs *cmd, const char *input) {
    int i = 0, buf_idx = 0;
    int in_single = 0, in_double = 0;
    cmd->argc = 0;
    char *buffer = malloc(strlen(input) + 1);
    if (!buffer)
        return;

    while (input[i] != '\0') {
        if (input[i] == '\\') {
            if (in_double) {
                if (input[i + 1] != '\0') {
                    if (input[i + 1] == '\"' || input[i + 1] == '\\' ||
                        input[i + 1] == '$' || input[i + 1] == '`') {
                        buffer[buf_idx++] = input[i + 1];
                        i += 2;
                        continue;
                    } else {
                        buffer[buf_idx++] = input[i++];
                        continue;
                    }
                }
            } else if (in_single == 1 && input[i + 1] != '\0') {
                buffer[buf_idx++] = input[i++];
                continue;
            }
            if (input[i + 1] != '\0') {
                buffer[buf_idx++] = input[i + 1];
                i += 2;
                continue;
            }
        }
        if (input[i] == '\'' && in_double == 0) {
            if (in_single == 1)
                in_single = 0;
            else if (in_single == 0)
                in_single = 1;
            i++;
            continue;
        }

        if (input[i] == '\"' && in_single == 0) {
            if (in_double == 1)
                in_double = 0;
            else if (in_double == 0)
                in_double = 1;
            i++;
            continue;
        }

        if (!(in_single == 1 || in_double == 1) && input[i] == ' ') {
            buffer[buf_idx] = '\0';
            cmd->argv[cmd->argc++] = strdup(buffer);
            free(buffer);
            buffer = malloc(strlen(input));
            if (!cmd->argv[cmd->argc - 1]) {
                for (int i = 0; i < cmd->argc; i++)
                    free(cmd->argv[i]);
                free(cmd);
                free(cmd->argv);
                free(buffer);
                cmd->argc = 0;
                return;
            }
            buf_idx = 0;
            while (input[i] == ' ')
                i++;
            continue;
        }

        buffer[buf_idx++] = input[i++];
    }

    if (buffer) {
        buffer[buf_idx] = '\0';
        cmd->argv[cmd->argc++] = strdup(buffer);
        if (!cmd->argv[cmd->argc - 1]) {
            for (int i = 0; i < cmd->argc; i++)
                free(cmd->argv[i]);
            free(cmd->argv);
            free(buffer);
            cmd->argc = 0;
            return;
        }
        free(buffer);
    }
}

CommandArgs *tokenize_input(const char *input) {
    if (!input || *input == '\0') {
        CommandArgs *cmd = malloc(sizeof(CommandArgs));
        if (!cmd)
            return NULL;

        cmd->argc = 0;
        cmd->argv = malloc(sizeof(char *));
        if (!cmd->argv) {
            free(cmd);
            return NULL;
        }
        cmd->argv[0] = NULL;
        cmd->stdout_file = cmd->stderr_file = NULL;
        return cmd;
    }

    char *copy = strdup(input);
    if (!copy)
        return NULL;

    char *trimmed = trim_whitespace(copy);
    if (*trimmed == '\0') {
        free(copy);
        CommandArgs *cmd = malloc(sizeof(CommandArgs));
        if (!cmd)
            return NULL;

        cmd->argc = 0;
        cmd->argv = malloc(sizeof(char *));
        if (!cmd->argv) {
            free(cmd);
            return NULL;
        }
        cmd->argv[0] = NULL;
        cmd->stdout_file = cmd->stderr_file = NULL;
        return cmd;
    }

    size_t len = strlen(trimmed);
    CommandArgs *cmd = malloc(sizeof(CommandArgs));
    if (!cmd) {
        free(copy);
        return NULL;
    }

    cmd->argv = malloc((len + 2) * sizeof(char *));
    if (!cmd->argv) {
        free(copy);
        free(cmd);
        return NULL;
    }

    make_tokens(cmd, trimmed);
    cmd->stdout_file = cmd->stderr_file = NULL;

    for (int i = 0; i < cmd->argc; i += 1) {
        if ((strcmp(">", cmd->argv[i]) == 0) ||
            (strcmp("1>", cmd->argv[i]) == 0)) {
            cmd->stdout_append = 1;
            cmd->stdout_file = strdup(cmd->argv[i + 1]);
            for (int j = i; j < cmd->argc; j += 1)
                free(cmd->argv[j]);
            cmd->argc = i;
            break;
        } else if (strcmp("2>", cmd->argv[i]) == 0) {
            cmd->stderr_append = 1;
            cmd->stderr_file = strdup(cmd->argv[i + 1]);
            for (int j = i; j < cmd->argc; j += 1)
                free(cmd->argv[j]);
            cmd->argc = i;
            break;
        }
    }

    cmd->argv[cmd->argc] = NULL;

    if (cmd->argc > 0 && cmd->argc < len) {
        char **trimmed_argv =
            realloc(cmd->argv, (cmd->argc + 1) * sizeof(char *));
        if (trimmed_argv) {
            cmd->argv = trimmed_argv;
        }
    }

    // for(int i = 0; i < cmd->argc; i += 1)
    //     printf("%s\n", cmd->argv[i]);

    free(copy);
    return cmd;
}

void free_command_args(CommandArgs *cmd) {
    if (!cmd)
        return;

    if (cmd->argv) {
        for (int i = 0; i < cmd->argc; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
    }
    free(cmd->stderr_file);
    free(cmd->stdout_file);
    free(cmd);
}
