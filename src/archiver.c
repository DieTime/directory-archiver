#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "../include/archiver.h"

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