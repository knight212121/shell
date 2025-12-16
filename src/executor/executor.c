#include "executor.h"
#include "../builtins/builtins.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

enum { BUF_SIZE = 4096 };

char* find_external_command(char *name) {
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
                if (strcmp(entry->d_name, name) == 0) {
                    char *full_path =
                        malloc(strlen(target_dir) + 2 + strlen(entry->d_name));
                    if (full_path == NULL) {
                        continue;
                    }
                    strcpy(full_path, target_dir);
                    strcat(full_path,
                           "/"); // Add windows compatibility for forward slash
                    strcat(full_path, entry->d_name);
                    if (access(full_path, X_OK) != -1) {
                        return full_path;
                        break;
                    } else {
                        free(full_path);
                    }
                }
            }
            closedir(dir);
            target_dir = strtok_r(NULL, PATH_LIST_SEPARATOR, &save_ptr);
        }
    }
    return NULL;
}

int execute_external_command(CommandArgs *cmd) {
    char *executable = find_external_command(cmd->argv[0]);
    if (executable) {
        int saved_std;
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork fail");
            exit(1);
        } else if (pid == 0) {
            static char *env_args[] = { NULL };
            if (cmd->stdout_file) {
                saved_std = dup(1);
                int file_desc;
                if (cmd->stdout_append == 1)
                    file_desc = open(cmd->stdout_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
                else
                    file_desc = open(cmd->stdout_file, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
                fflush(stdout);
                dup2(file_desc, 1);
                execve(executable, cmd->argv, env_args);
            } else if (cmd->stderr_file) {
                saved_std = dup(2);
                int file_desc;
                if (cmd->stderr_append == 1)
                    file_desc = open(cmd->stderr_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
                else
                    file_desc = open(cmd->stderr_file, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
                fflush(stderr);
                dup2(file_desc, 2);
                execve(executable, cmd->argv, env_args);
            } else
                execve(executable, cmd->argv, env_args);
            perror("execve");
        } else {
            wait(NULL);
            if (cmd->stdout_file)
                dup2(saved_std, 1);
            if (cmd->stderr_file)
                dup2(saved_std, 2);
        }
        return 1;
    }
    return 0;
}

void execute_command(CommandArgs *cmd) {
    if (execute_builtin_command(cmd) == 1)
        return;
    if (execute_external_command(cmd) == 1)
        return;
    printf("%s: command not found\n", cmd->argv[0]);
}
