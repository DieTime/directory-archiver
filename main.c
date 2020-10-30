#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "include/archiver.h"

int main(int argc, char** argv) {
    // Too few arguments error
    if (argc < 3) {
        printf("[ERROR] Possible archiver arguments:\n");
        printf("        --pack   <source folder path> <output archive path>\n");
        printf("        --unpack <archive path> <output folder path (optional)>\n");
	return 1;
    }

    // Getting arguments
    char* action = argv[1];
    char* source = argv[2];
    char* output = "";

    // Check the source path to access
    if (access(source, F_OK) == -1) {
        printf("[ERROR] Couldn't %s. %s doesn't exists.\n", action + 2, source);
        return 1;
    }

    if (argc == 3) {
        // Set default output if no output argument
        if (strcmp(action, "--pack") == 0) {
            output = "archive.tar";
        } else {
            output = "";
        }
    } else {
        // Get output path if args > 3
        output = argv[3];
    }

    // Pack or unpack depending on the flag
    if (strcmp(action, "--pack") == 0) {
        pack(source, output);
    } else {
        unpack(source, output);
    }

    // Echo success status
    printf("[SUCCESS] %s was successfully %sed.\n", output, action + 2);
    return 0;
}
