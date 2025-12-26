#ifndef COMMON_H
#define COMMON_H

typedef struct {
    int argc;
    char **argv;

    char *stdout_file;
    int stdout_append;

    char *stderr_file;
    int stderr_append;

} CommandArgs;

typedef struct {
    int count;
    CommandArgs **cmds;
} Pipeline;

extern char *history_file;

#endif
