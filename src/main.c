#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    while (1) {
        // Flush after every printf
        setbuf(stdout, NULL);

        printf("$ ");

        char command[1024];
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        printf("%s: command not found\n", command);
    }

    return 0;
}
