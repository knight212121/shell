#include "builtins.h"
#include "history.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

void cd(CommandArgs *cmd) {
    if (cmd->argc > 2) {
        printf("cd: too many arguments\n");
        return;
    }
    char *full_path;
    if (cmd->argv[1][0] == '~') {
        char *home = getenv("HOME");
        if (!home) {
            printf("$HOME not set");
            return;
        }
        full_path = malloc(strlen(home) + strlen(cmd->argv[1]) + 1);
        if (!full_path) {
            printf("malloc error");
            return;
        }
        strcpy(full_path, home);
        strcat(full_path, cmd->argv[1] + 1);
    } else {
        full_path = strdup(cmd->argv[1]);
    }

    if (chdir(full_path) == -1) {
        printf("cd: %s: No such file or directory\n", full_path);
    }
    free(full_path);
}

void exit_shell(CommandArgs *cmd) {
    if (cmd->argc > 2) {
        printf("Too many arguments\n");
        return;
    }
    int code = 0;
    if (cmd->argv[1]) {
        code = atoi(cmd->argv[1]);
    }
    exit(code);
}

void echo(CommandArgs *cmd) {
    for (int i = 1; i < cmd->argc; i += 1) {
        if (i > 1)
            write(STDOUT_FILENO, " ", 1);
        write(STDOUT_FILENO, cmd->argv[i], strlen(cmd->argv[i]));
    }
    write(STDOUT_FILENO, "\n", 1);
}

void type(CommandArgs *cmd) {
    int i;
    for (i = 1; i < cmd->argc; i += 1) {
        int j, found = 0;
        for (j = 0; j < command_count; j += 1) {
            if (strcmp(commands[j].name, cmd->argv[i]) == 0) {
                printf("%s is a shell builtin\n", cmd->argv[i]);
                found = 1;
                break;
            }
        }
        if (found == 0) {
            char *PATH = strdup(getenv("PATH"));
            if (PATH) {
                char *save_ptr = NULL;
                char *target_dir =
                    strtok_r(PATH, PATH_LIST_SEPARATOR, &save_ptr);
                while (target_dir) {
                    struct dirent *entry;
                    DIR *dir = opendir(target_dir);
                    if (dir == NULL) {
                        continue;
                    }
                    while ((entry = readdir(dir)) != NULL) {
                        if (strcmp(entry->d_name, cmd->argv[i]) == 0) {
                            char *full_path = malloc(strlen(target_dir) + 2 +
                                                     strlen(entry->d_name));
                            if (full_path == NULL) {
                                continue;
                            }
                            strcpy(full_path, target_dir);
                            strcat(full_path,
                                   "/"); // Probably add windows compatibility
                                         // for forward slash
                            strcat(full_path, entry->d_name);
                            if (access(full_path, X_OK) != -1) {
                                printf("%s is %s\n", cmd->argv[i], full_path);
                                free(full_path);
                                found = 1;
                                break;
                            } else {
                                free(full_path);
                            }
                        }
                    }
                    closedir(dir);
                    if (found == 1)
                        break;
                    target_dir = strtok_r(NULL, PATH_LIST_SEPARATOR, &save_ptr);
                }
            }
            if (found == 0)
                printf("%s: not found\n", cmd->argv[i]);
            free(PATH);
        }
    }
}

void pwd(CommandArgs *cmd) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else {
        perror("getcwd() error");
    }
}

void shell_history(CommandArgs *cmd) {
    if (cmd->argc == 1) {
        print_history(-1);
        return;
    }

    if (cmd->argc == 2) {
        for(int i = 0; i < strlen(cmd->argv[1]); i += 1) {
            if (!(cmd->argv[1][i] >= '0' && cmd->argv[1][i] <= '9')) {
                printf("history: %s: numeric argument required\n", cmd->argv[1]);
                return;
            }
        }
        print_history(atoi(cmd->argv[1]));
        return;
    }

    if (cmd->argc == 3 && strcmp(cmd->argv[1], "-r") == 0) {
        if (read_history(cmd->argv[2]) != 0)
            printf("Error reading file");
        return;
    }

    if (cmd->argc == 3 && strcmp(cmd->argv[1], "-w") == 0) {
        if (write_history(cmd->argv[2]) != 0)
            printf("Error writing file");
        return;
    }

    if (cmd->argc == 3 && strcmp(cmd->argv[1], "-a") == 0) {
        if (append_history(history_length, cmd->argv[2]) != 0)
            printf("Error writing file");
        return;
    }
}

ShellCommand commands[] = {
    {"echo", echo, 0}, {"exit", exit_shell, 1}, {"type", type, 0},
    {"pwd", pwd, 0},   {"cd", cd, 1},           {"history", shell_history, 1},
};

int command_count = sizeof(commands) / sizeof(commands[0]);

int is_parent_builtin(char *cmd) {
    for (int i = 0; i < command_count; i += 1) {
        if (strcmp(commands[i].name, cmd) == 0 &&
            commands[i].parent_builtin == 1) {
            return 1;
        }
    }
    return 0;
}

int execute_builtin_command(CommandArgs *cmd) {
    for (int i = 0; i < command_count; i += 1) {
        if (strcmp(commands[i].name, cmd->argv[0]) == 0) {
            commands[i].func(cmd);
            return 1;
        }
    }
    return 0;
}
