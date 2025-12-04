#include "executor.h"
#include "../builtins/builtins.h"

void execute_command(CommandArgs* cmd) {
    if (execute_builtin_command(cmd) == 1) {
        return;
    }
}
