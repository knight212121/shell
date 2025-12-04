#include "executor.h"
#include "../builtins/builtins.h"
#include <stdio.h>

void execute_command(CommandArgs* cmd) {
    if (execute_builtin_command(cmd) == 1) {
        return;
    }
    printf("%s: command not found\n", cmd -> argv[0]);
}
