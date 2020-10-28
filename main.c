#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "include/archiver.h"

int main(int argc, char** argv) {
    // Getting arguments
    char* flag = argv[1];
    char* path = argv[2];
    char* output = argv[3];

    // Checking the number of arguments
    if (argc != 4) {
        printf("[ERROR] Archiver takes only 3 parameters: --pack(--unpack) <dir_path> <output name>.\n");
        return 1;
    }

    // Checking the first argument
    if (strcmp(flag, "--pack") != 0 && strcmp(flag, "--unpack") != 0) {
        printf("[ERROR] Invalid argument: %s. Maybe you mean --pack or --unpack?\n", flag);
        return 1;
    }

    // Check the source path to access
    if (access(path, F_OK) == -1) {
        printf("[ERROR] Couldn't %s. Source file %s doesn't exists.\n", flag + 2, path);
        return 1;
    }

    // Check the output path to access
    if (access(output, F_OK) != -1) {
        // Send warning message about overwriting
        printf("[WARNING] Couldn't %s. File %s is already exists.\n", flag + 2, output);

        // Asking the user to confirm overwriting
        if (strcmp(flag, "--pack") == 0) {
            printf("          Do you want to overwrite %s output file?\n", output);
            printf("          Please enter 'y' or 'n': ");

            // Waiting for confirmation to overwrite if user want pack
            int input;
            while((input = _getch())) {
                if (input == 'y' || input == 'n') {
                    break;
                }
            }

            if (input == 'y') {
                // Try remove output file if user agree
                if (unlink(output) == -1) {
                    printf("\n\n[ERROR] Couldn't remove %s output file.\n", output);
                    return 1;
                }
                printf("\n\n");
            } else {
                // Exit if user doesn't agree
                printf("\n\n[ABORT] %s output file has not been overwritten.\n", output);
                return 1;
            }
        } else {
            // Exit if user want unpack and directory is already exists
            return 1;
        }
    }

    // Pack or unpack depending on the flag
    if (strcmp(flag, "--pack") == 0) {
        pack(path, output);
    } else {
        // ...unpack
    }

    printf("\n[SUCCESS] %s output file was successfully %sed.\n", output, flag + 2);
    return 0;
}