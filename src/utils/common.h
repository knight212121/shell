#ifndef COMMON_H
#define COMMON_H

typedef struct {
    int argc;
    char **argv;

    char *stdin_file;
    char *stdout_file;
    int append;
} CommandArgs;

#endif
