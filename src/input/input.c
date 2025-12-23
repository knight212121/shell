#include "input.h"
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

void make_tokens(char **tokens, const char *input) {
    int i = 0, buf_idx = 0;
    int in_single = 0, in_double = 0;
    int token_idx = 0;
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
            tokens[token_idx] = strdup(buffer);
            if (!tokens[token_idx]) {
                for (int i = 0; i < token_idx; i += 1) {
                    free(tokens[i]);
                }
                tokens = NULL;
                return;
            }
            token_idx += 1;
            free(buffer);
            buffer = malloc(strlen(input));
            if (!buffer) {
                tokens = NULL;
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
        tokens[token_idx] = strdup(buffer);
        if (!tokens[token_idx]) {
            for (int i = 0; i < token_idx; i += 1) {
                free(tokens[i]);
            }
            tokens = NULL;
            return;
        }
        token_idx += 1;
        tokens[token_idx] = NULL;
        free(buffer);
    }
    tokens = realloc(tokens, sizeof(char *) * token_idx + 1);
    tokens[token_idx] = NULL;
}

Pipeline *tokenize_input(char *input) {
    int len = strlen(input);
    if (!input || *input == '\0')
        return NULL;

    input = trim_whitespace(input);
    if (*input == '\0' || !input)
        return NULL;

    char **tokens = malloc(sizeof(char *) * strlen(input));
    make_tokens(tokens, input);
    if (tokens == NULL)
        return NULL;

    CommandArgs *cmd = malloc(sizeof(CommandArgs));
    if (!cmd)
        return NULL;
    cmd->argv = malloc(sizeof(char *) * len);
    cmd->stdout_file = cmd->stderr_file = NULL;
    cmd->argc = 0;

    Pipeline *p = malloc(sizeof(Pipeline));
    p->cmds = malloc(sizeof(CommandArgs) * len + 1);
    if (!p)
        return NULL;
    p->count = 0;

    int iterator = 0;
    for (int iterator = 0; tokens[iterator] != NULL; iterator += 1) {
        if ((strcmp(">", tokens[iterator]) == 0) ||
            (strcmp("1>", tokens[iterator]) == 0)) {
            cmd->stdout_append = 1;
            cmd->stdout_file = strdup(tokens[iterator + 1]);
            iterator += 1;
        } else if (strcmp("2>", tokens[iterator]) == 0) {
            cmd->stderr_append = 1;
            cmd->stderr_file = strdup(tokens[iterator + 1]);
            iterator += 1;
        } else if ((strcmp(">>", tokens[iterator]) == 0) ||
                   (strcmp("1>>", tokens[iterator]) == 0)) {
            cmd->stdout_append = 2;
            cmd->stdout_file = strdup(tokens[iterator + 1]);
            iterator += 1;
        } else if (strcmp("2>>", tokens[iterator]) == 0) {
            cmd->stderr_append = 2;
            cmd->stderr_file = strdup(tokens[iterator + 1]);
            iterator += 1;
        } else if (strcmp("|", tokens[iterator]) == 0) {
            cmd->argv = realloc(cmd->argv, sizeof(char *) * cmd->argc + 1);
            cmd->argv[cmd->argc] = NULL;

            p->cmds[p->count++] = cmd;

            cmd = malloc(sizeof(CommandArgs));
            if (!cmd)
                return NULL;

            cmd->argv = malloc(sizeof(char *) * len + 1);
            cmd->stdout_file = cmd->stderr_file = NULL;
            cmd->argc = 0;
        } else {
            cmd->argv[cmd->argc++] = strdup(tokens[iterator]);
            if (!tokens[cmd->argc - 1]) {
                for (int j = 0; j < cmd->argc; j += 1)
                    free(tokens[j]);
                free(tokens);
                free(cmd);
                free(p);
                return NULL;
            }
        }
    }
    cmd->argv = realloc(cmd->argv, sizeof(CommandArgs *) * cmd->argc + 1);
    cmd->argv[cmd->argc] = NULL;

    p->cmds[p->count++] = cmd;

    p->cmds = realloc(p->cmds, sizeof(CommandArgs) * p->count);
    p->cmds[p->count] = NULL;

    for (int i = 0; tokens[i] != NULL; i += 1) {
        free(tokens[i]);
    }

    free(tokens);

    return p;
}

void free_command_args(Pipeline *pipes) {
    if (!pipes)
        return;

    for (int i = 0; i < pipes->count; i += 1) {
        CommandArgs* cmd = pipes->cmds[i];
        for (int j = 0; j < cmd->argc; j += 1) {
            free(cmd->argv[j]);
        }
        free(cmd->argv);
        free(cmd->stderr_file);
        free(cmd->stdout_file);
        free(cmd);
    }
    free(pipes->cmds);
    free(pipes);
}
