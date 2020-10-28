#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void print_dir(char* path);

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

void print_dir(char* path) {
    // Open directory by path
    DIR *dir = opendir(path);

    // Stop if we couldn't open directory
    if (dir == NULL) {
        return;
    }

    // Get the next directory entry
    struct dirent *entry;

    // While the next directory entry exists
    while ((entry = readdir(dir)) != NULL) {
        // Skip if entry name is . or ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Get current entry path
        char file_path[1024] = {0};
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // Print entry files recursively if the entry is directory
            print_dir(file_path);
        }
        else {
            // Print entry path if the entry is file
            printf("%s\n", file_path);
        }
    }

    // Close directory
    closedir(dir);
}
