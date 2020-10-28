#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "include/archiver.h"

int main(int argc, char** argv) {
    // Getting arguments
    char* flag = argv[1];
    char* path = argv[2];

    // Checking the number of arguments
    if (argc != 3) {
        printf("[ERROR] Program takes only 2 parameters: --pack(--unpack) and dir_path.");
        return 1;
    }

    // Checking the first argument
    if (strcmp(flag, "--pack") != 0 && strcmp(flag, "--unpack") != 0) {
        printf("[ERROR] Invalid argument: %s. Maybe you mean --pack or --unpack?", flag);
        return 1;
    }

    // Check the directory path to access
    if (access(path, F_OK) == -1) {
        printf("[ERROR] Couldn't access %s. Maybe this path doesn't exists.", path);
        return 1;
    }

    // Get list of files in directory
    print_dir(path);

    return 0;
}