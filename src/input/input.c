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

char **make_tokens(const char *input) {
    int i = 0, buf_idx = 0, capacity = 10;
    int in_single = 0, in_double = 0;
    int token_idx = 0;
    char **tokens = malloc(sizeof(char *) * capacity);
    if (!tokens)
        return NULL;
    char *buffer = malloc(strlen(input) + 1);
    if (!buffer) {
        free(tokens);
        return NULL;
    }

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
            if (token_idx >= capacity) {
                capacity += capacity;
                char **temp = realloc(tokens, sizeof(char *) * capacity);
                if (!temp) {
                    for (int i = 0; i < token_idx; i += 1) {
                        free(tokens[i]);
                    }
                    free(tokens);
                    free(buffer);
                    return NULL;
                }
                tokens = temp;
            }
            tokens[token_idx] = strdup(buffer);
            if (!tokens[token_idx]) {
                for (int i = 0; i < token_idx; i += 1) {
                    free(tokens[i]);
                }
                free(buffer);
                free(tokens);
                return NULL;
            }
            token_idx += 1;
            free(buffer);
            buffer = malloc(strlen(input) + 1);
            if (!buffer) {
                for (int i = 0; i < token_idx; i += 1) {
                    free(tokens[i]);
                }
                free(tokens);
                free(buffer);
                return NULL;
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
            free(tokens);
            free(buffer);
            return NULL;
        }
        token_idx += 1;
        tokens[token_idx] = NULL;
        free(buffer);
    }
    tokens[token_idx] = NULL;
    return tokens;
}

Sequence *tokenize_input(char *input) {
    int len = 20, argv_cap = 20, cmds_cap = 20, sequence_cap = 20;
    if (!input || *input == '\0')
        return NULL;

    input = trim_whitespace(input);
    if (*input == '\0' || !input)
        return NULL;

    char **tokens;
    tokens = make_tokens(input);
    if (tokens == NULL)
        return NULL;

    CommandArgs *cmd = malloc(sizeof(CommandArgs));
    if (!cmd) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    cmd->argv = malloc(sizeof(char *) * argv_cap);
    if (!cmd->argv) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    cmd->stdout_file = NULL;
    cmd->stderr_file = NULL;
    cmd->stdout_append = 0;
    cmd->stderr_append = 0;
    cmd->argc = 0;

    Pipeline *p = malloc(sizeof(Pipeline));
    if (!p) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    p->cmds = malloc(sizeof(CommandArgs *) * cmds_cap);
    if (!p->cmds) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    p->count = 0;

    Sequence *s = malloc(sizeof(Sequence));
    if (!s) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    s->pipelines = malloc(sizeof(Pipeline *) * sequence_cap);
    if (!s->pipelines) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    s->count = 0;

    for (int iterator = 0; tokens[iterator] != NULL; iterator += 1) {
        if ((strcmp(">", tokens[iterator]) == 0) ||
            (strcmp("1>", tokens[iterator]) == 0)) {
            cmd->stdout_append = 1;
            cmd->stdout_file = strdup(tokens[iterator + 1]);
            if (!cmd->stdout_file) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }
            iterator += 1;
        } else if (strcmp("2>", tokens[iterator]) == 0) {
            cmd->stderr_append = 1;
            cmd->stderr_file = strdup(tokens[iterator + 1]);
            if (!cmd->stderr_file) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }
            iterator += 1;
        } else if ((strcmp(">>", tokens[iterator]) == 0) ||
                   (strcmp("1>>", tokens[iterator]) == 0)) {
            cmd->stdout_append = 2;
            cmd->stdout_file = strdup(tokens[iterator + 1]);
            if (!cmd->stdout_file) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }
            iterator += 1;
        } else if (strcmp("2>>", tokens[iterator]) == 0) {
            cmd->stderr_append = 2;
            cmd->stderr_file = strdup(tokens[iterator + 1]);
            if (!cmd->stderr_file) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }
            iterator += 1;
        } else if ((strcmp(";", tokens[iterator]) == 0) ||
                   (strcmp("|", tokens[iterator]) == 0)) {
            char **cmd_temp =
                realloc(cmd->argv, sizeof(char *) * (cmd->argc + 1));
            if (!cmd_temp) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }
            cmd->argv = cmd_temp;
            cmd->argv[cmd->argc] = NULL;
            if (p->count >= cmds_cap) {
                cmds_cap *= 2;
                CommandArgs **temp =
                    realloc(p->cmds, sizeof(CommandArgs *) * cmds_cap);
                if (!temp) {
                    for (int i = 0; tokens[i] != NULL; i += 1)
                        free(tokens[i]);
                    return NULL;
                }
                p->cmds = temp;
            }
            p->cmds[p->count++] = cmd;
            p->cmds[p->count] = NULL;

            cmd = malloc(sizeof(CommandArgs));
            if (!cmd) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }

            argv_cap = 20;
            cmd->argv = malloc(sizeof(char *) * argv_cap);
            if (!cmd->argv) {
                for (int i = 0; tokens[i] != NULL; i += 1)
                    free(tokens[i]);
                return NULL;
            }
            cmd->stdout_file = NULL;
            cmd->stderr_file = NULL;
            cmd->stdout_append = 0;
            cmd->stderr_append = 0;
            cmd->argc = 0;

            if (strcmp(";", tokens[iterator]) == 0) {
                if (s->count >= sequence_cap) {
                    sequence_cap *= 2;
                    Pipeline **temp =
                        realloc(s->pipelines, sizeof(Pipeline *) * sequence_cap);
                    if (!temp) {
                        for (int i = 0; tokens[i] != NULL; i += 1)
                            free(tokens[i]);
                        return NULL;
                    }
                    s->pipelines = temp;
                }
                s->pipelines[s->count++] = p;
                s->pipelines[s->count] = NULL;

                p = malloc(sizeof(Pipeline));
                if (!p) {
                    for (int i = 0; tokens[i] != NULL; i += 1)
                        free(tokens[i]);
                    return NULL;
                }
                cmds_cap = 20;
                p->cmds = malloc(sizeof(CommandArgs *) * cmds_cap);
                if (!p->cmds) {
                    for (int i = 0; tokens[i] != NULL; i += 1)
                        free(tokens[i]);
                    for (int i = 0; p->cmds[i] != NULL; i += 1)
                        free(p->cmds[i]);
                    return NULL;
                }
                p->count = 0;
            }
        } else {
            if (cmd->argc >= argv_cap) {
                argv_cap *= 2;
                char **tmp = realloc(cmd->argv, sizeof(char *) * argv_cap);
                if (!tmp) {
                    for (int i = 0; tokens[i] != NULL; i += 1)
                        free(tokens[i]);
                    return NULL;
                }
                cmd->argv = tmp;
            }
            cmd->argv[cmd->argc++] = strdup(tokens[iterator]);
            if (!cmd->argv[cmd->argc - 1]) {
                for (int j = 0; j < cmd->argc; j += 1)
                    free(cmd->argv[j]);
                free(cmd->argv);
                free(cmd);
                free(p);
                return NULL;
            }
        }
    }
    char **argv_temp = realloc(cmd->argv, sizeof(char *) * (cmd->argc + 1));
    if (!argv_temp) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    cmd->argv = argv_temp;
    cmd->argv[cmd->argc] = NULL;

    p->cmds[p->count++] = cmd;

    CommandArgs **cmds_temp =
        realloc(p->cmds, sizeof(CommandArgs *) * (p->count + 1));
    if (!cmds_temp) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        return NULL;
    }
    p->cmds = cmds_temp;
    p->cmds[p->count] = NULL;

    Pipeline **pipe_temp =
        realloc(s->pipelines, sizeof(Pipeline *) * (p->count + 1));
    if (!pipe_temp) {
        for (int i = 0; tokens[i] != NULL; i += 1)
            free(tokens[i]);
        for (int i = 0; p->cmds[i] != NULL; i += 1)
            free(p->cmds[i]);
        return NULL;
    }
    s->pipelines = pipe_temp;
    s->pipelines[s->count++] = p;
    s->pipelines[s->count] = NULL;

    for (int i = 0; tokens[i] != NULL; i += 1) {
        free(tokens[i]);
    }

    free(tokens);

    return s;
}

void free_command_args(Sequence *sequence) {
    if (!sequence)
        return;

    for (int i = 0; i < sequence->count; i += 1) {
        Pipeline *pipes = sequence->pipelines[i];
        for (int i = 0; i < pipes->count; i += 1) {
            CommandArgs *cmd = pipes->cmds[i];
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
    free(sequence);
    free(history_file);
}
