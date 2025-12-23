#include "builtins.h"
#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
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

ShellCommand commands[] = {
    {"echo", echo, 0}, {"exit", exit_shell, 1}, {"type", type, 0},
    {"pwd", pwd, 0},   {"cd", cd, 1},
};

int command_count = sizeof(commands) / sizeof(commands[0]);

int is_parent_builtin(char *cmd) {
    for (int i = 0; i < command_count; i += 1) {
        if (strcmp(commands[i].name, cmd) == 0 && commands[i].parent_builtin == 1) {
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
