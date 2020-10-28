#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // Checking the number of arguments
    if (argc != 3) {
        printf("[ERROR] Program takes only 2 parameters: --pack(--unpack) and dir_path.");
        return 1;
    }

    // Checking the first argument
    if (strcmp(argv[1], "--pack") != 0 && strcmp(argv[1], "--unpack") != 0) {
        printf("[ERROR] Invalid argument: %s. Maybe you mean --pack or --unpack?", argv[1]);
        return 1;
    }

    if (access(argv[2], F_OK) == -1) {
        printf("[ERROR] Couldn't access %s. Maybe this path doesn't exists.", argv[2]);
        return 1;
    }

    for (int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    return 0;
}
