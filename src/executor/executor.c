#include "executor.h"
#include "../builtins/builtins.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

enum { BUF_SIZE = 4096 };
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

char *find_external_command(char *name) {
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
        static char *env_args[] = {NULL};
        execve(executable, cmd->argv, env_args);
        perror("execve");
        return 1;
    }
    return 0;
}

void execute_command(Pipeline *pipes) {
    int prev_read_fd = -1;
    int pipefd[2];
    for (int i = 0; i < pipes->count; i += 1) {
        if (i < pipes->count - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(1);
            }
        }
        CommandArgs *cmd = pipes->cmds[i];
        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork fail");
            exit(1);
        } else if (pid == 0) {
            if (prev_read_fd != -1) {
                dup2(prev_read_fd, STDIN_FILENO);
                close(prev_read_fd);
            }
            if (i < pipes->count - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            if (cmd->stdout_file) {
                int file_desc;
                if (cmd->stdout_append == 1)
                    file_desc =
                        open(cmd->stdout_file, O_CREAT | O_TRUNC | O_WRONLY,
                             S_IRUSR | S_IWUSR);
                else if (cmd->stdout_append == 2)
                    file_desc =
                        open(cmd->stdout_file, O_CREAT | O_APPEND | O_WRONLY,
                             S_IRUSR | S_IWUSR);
                if (file_desc == -1) {
                    perror(cmd->stdout_file);
                    _exit(1);
                }
                dup2(file_desc, STDOUT_FILENO);
                close(file_desc);
            }
            if (cmd->stderr_file) {
                int file_desc;
                if (cmd->stderr_append == 1)
                    file_desc =
                        open(cmd->stderr_file, O_CREAT | O_TRUNC | O_WRONLY,
                             S_IRUSR | S_IWUSR);
                else if (cmd->stderr_append == 2)
                    file_desc =
                        open(cmd->stderr_file, O_CREAT | O_APPEND | O_WRONLY,
                             S_IRUSR | S_IWUSR);
                if (file_desc == -1) {
                    perror(cmd->stderr_file);
                    _exit(1);
                }
                dup2(file_desc, STDERR_FILENO);
                close(file_desc);
            }
            if (execute_builtin_command(pipes->cmds[i]) == 1)
                _exit(0);
            else if (execute_external_command(pipes->cmds[i]) == 1)
                _exit(0);
            else {
                fprintf(stderr, "%s: command not found\n",
                        pipes->cmds[i]->argv[0]);
                _exit(127);
            }
            _exit(1);
        } else {
            if (i < pipes->count - 1) {
                close(pipefd[1]);
                if (prev_read_fd != -1)
                    close(prev_read_fd);
                prev_read_fd = pipefd[0];
            }
        }
    }

    for (int i = 0; i < pipes->count; i += 1)
        wait(NULL);
}
