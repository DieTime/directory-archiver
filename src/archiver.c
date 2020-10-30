#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termio.h>
#include <unistd.h>

#include "../include/archiver.h"

void pack(char* dir_path, char* archive_path) {
    int archive;      // Archive descriptor
    u64 root_dir_len; // Length of root directory name

    // Check if archive path is already exists
    if (access(archive_path, F_OK) != -1) {
        // Send warning message about overwriting
        printf("[WARNING] Couldn't pack. %s is already exists.\n", archive_path);
        printf("          Do you want to overwrite %s?\n", archive_path);
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
            if (unlink(archive_path) == -1) {
                printf("\n\n[ERROR] Couldn't remove %s.\n", archive_path);
                exit(1);
            }
            printf("\n\n");
        } else {
            // Exit if user doesn't agree
            printf("\n\n[ABORT] %s has not been overwritten.\n", archive_path);
            exit(1);
        }
    }

    // Create output archive
    archive = open(archive_path, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    if (archive == -1) {
        printf("[ERROR] Couldn't create %s.\n", archive_path);
        exit(1);
    }

    // Write to archive file MAGIC bites
    if (write(archive, MAGIC, MAGIC_LENGTH) != MAGIC_LENGTH) {
        printf("[ERROR] Couldn't write magic header to %s.\n", archive_path);
        exit(1);
    }

    // Write length of name of root directory.
    // If the user wants to distribute with a different
    // name, we can change the root directory.
    root_dir_len = strlen(dir_path);
    if (write(archive, &root_dir_len, u64sz) != u64sz) {
        printf("[ERROR] Couldn't write root path length to %s.\n", archive_path);
        exit(1);
    }

    // Pack all files to archive
    _pack(archive, dir_path);

    // New line for pretty printing
    printf("\n");

    // Close output archive
    if (close(archive) == -1) {
        printf("[ERROR] Couldn't close %s.\n", archive_path);
        exit(1);
    }
}

void _pack(int archive, char* src_path) {
    // Open directory by path
    DIR *dir = opendir(src_path);

    // If directory path turned out to be a file path
    if (dir == NULL) {
        // Print error
        printf("[ERROR] %s is not a directory.\n", src_path);

        // Try close archive
        if (close(archive) == -1) {
            printf("[ERROR] Couldn't close archive.\n");
        }

        // Exit from program
        exit(1);
    }

    // Write info about folder to archive
    _pack_info(archive, FOLDER_NAME, src_path);

    // Get the next directory entry
    struct dirent *entry;

    // While the next directory entry exists
    while ((entry = readdir(dir)) != NULL) {
        // Skip if entry name is . or ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Get current entry path
        char file_path[MAX_PATH_LENGTH] = {0};
        snprintf(file_path, sizeof(file_path), "%s/%s", src_path, entry->d_name);
        _remove_extra_slash(file_path);

        if (entry->d_type == DT_DIR) {
            // Pack entry files recursively if the entry is directory
            _pack(archive, file_path);
        }
        else {
            // Write info about file to archive
            _pack_info(archive, FILE_NAME, file_path);

            // Write file content to archive
            _pack_content(archive, file_path);

            // Echo packing process
            printf("[PACKED] %s\n", file_path);
        }
    }

    // Close directory
    if (closedir(dir) == -1) {
        printf("[ERROR] Couldn't close %s.\n", src_path);
        exit(1);
    }
}

void _pack_info(int fd, path_type p_type, char* path) {
    // Convert to uint8_t filetype
    u8 type = (u8)p_type;

    // Error flag
    int error = 1;

    do {
        // Write to archive path type (file or folder)
        if (write(fd, &type, u8sz) != u8sz) {
            printf("[ERROR] Couldn't write file type to archive.\n");
            break;
        }

        // Write to archive file path length
        u64 file_path_len = strlen(path) + 1;
        if (write(fd, &file_path_len, u64sz) != u64sz) {
            printf("[ERROR] Couldn't write path length to archive.\n");
            break;
        }

        // Write to archive file path
        if (write(fd, path, file_path_len) != file_path_len) {
            printf("[ERROR] Couldn't write path length to archive.\n");
            break;
        }

        error = 0;
    } while (0);

    if (error) {
        if (close(fd) == -1) {
            printf("[ERROR] Couldn't close archive.\n");
        }
        exit(1);
    }
}

void _pack_content(int fd, char* path) {
    int error = 1;     // Error flag
    int file;          // File descriptor
    off_t size;        // Size of file content
    u64 file_size;     // Unsigned size of file content
    char content_byte; // Byte for copying content to archive

    // Open source file
    file = open(path, O_RDONLY, S_IRUSR);
    if (file == -1) {
        printf("[ERROR] Couldn't open %s for reading.\n", path);
        close(fd);
        exit(1);
    }

    do {
        // Get size of file
        size = lseek(file, 0, SEEK_END);
        if (size == -1) {
            printf("[ERROR] Couldn't get %s length.\n", path);
            break;
        }
        file_size = (u64)(size);

        // Seek back to the beginning
        if (lseek(file, 0, SEEK_SET) == -1) {
            printf("[ERROR] Couldn't set cursor in %s to start.\n", path);
            break;
        }

        // Write content size to archive
        if (write(fd, &file_size, u64sz) != u64sz) {
            printf("[ERROR] Couldn't write size of %s to archive.\n", path);
            break;
        }

        // Read content of source file
        for (u64 num_bytes = 0; num_bytes < file_size; num_bytes++) {
            if (read(file, &content_byte, 1) != 1) {
                printf("[ERROR] Couldn't read content of %s.\n", path);
                break;
            }

            // Write content of source to archive
            if (write(fd, &content_byte, 1) != 1) {
                printf("[ERROR] Couldn't write content of %s to archive.\n", path);
                break;
            }
        }

        error = 0;
    } while (0);

    // Fre archive descriptor if error
    if (error && close(fd) == -1) {
        printf("[ERROR] Couldn't close archive.\n");
    }

    // Close source file
    if (close(file) == -1) {
        printf("[ERROR] Couldn't close %s.\n", path);
        exit(1);
    }

    // Exit from program if error
    if (error) {
        exit(1);
    }
}

void unpack(char* archive_path, char* out_file) {
    int error = 1;            // Error flag
    int archive, file;        // File descriptors
    char magic[MAGIC_LENGTH]; // Buffer for MAGIC BYTES
    u64 root_dir_len;         // Length of name of root directory for replacing
    u8 type;                  // Type of path (file or folder)
    char* path = NULL;        // Buffer for file path in archive
    u64 path_len;             // Length of file path
    char content_byte;        // Variable for reading file content
    u64 content_len;          // Length of file content in archive
    int read_status;          // Status of reading from archive
    int write_status;         // Status of writing from archive

    // Try to open archive file
    archive = open(archive_path, O_RDONLY);
    if (archive == -1) {
        printf("[ERROR] Couldn't open %s.\n", archive_path);
        exit(1);
    }

    do {
        // Read MAGIC bytes and compare with original
        if (read(archive, &magic, MAGIC_LENGTH) != MAGIC_LENGTH || strcmp(magic, MAGIC) != 0) {
            printf("[ERROR] Source file %s is not an archive.\n", archive_path);
            break;
        }

        // Read length of root directory name from archive
        if (read(archive, &root_dir_len, u64sz) != u64sz) {
            printf("[ERROR] Couldn't read root folder length from %s.\n", archive_path);
            break;
        }

        error = 0;
    } while (0);

    // Cleanup memory and exit if error found
    if (error) {
        close(archive);
        exit(0);
    }

    // Start getting files from archive
    while (read(archive, &type, u8sz) == u8sz) {
        error = 1;

        // Read path length from archive
        if (read(archive, &path_len, u64sz) != u64sz) {
            printf("[ERROR] Couldn't read file path length from %s.\n", archive_path);
            break;
        }

        // Allocate memory for path
        path = (char*)malloc(path_len);
        if (path == NULL) {
            printf("[ERROR] Couldn't allocate memory for file path with %lu length.\n", path_len);
            break;
        }

        // Read path from archive
        if (read(archive, path, path_len) != path_len) {
            printf("[ERROR] Couldn't read file path from %s.\n", archive_path);
            free(path);
            break;
        }

        path = _rename_root(path, root_dir_len, out_file);
        if (path == NULL) {
            printf("[ERROR] Couldn't unpack archive with new name.\n");
            break;
        }

        // Check if path is already exists
        if (access(path, F_OK) != -1) {
            printf("[ERROR] Couldn't unpack archive. %s is already exists.\n", path);
            free(path);
            break;
        }

        if ((path_type)type == FILE_NAME) {
            // Get content length of file
            if (read(archive, &content_len, u64sz) != u64sz) {
                printf("[ERROR] Couldn't read file content length from %s.\n", archive_path);
                free(path);
                break;
            }

            // Open file for writing
            file = open(path, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
            if (file == -1) {
                printf("[ERROR] Couldn't create %s.\n", path);
                free(path);
                break;
            }

            // Write content from archive to file
            for (u64 num_bytes = 0; num_bytes < content_len; num_bytes++) {
                read_status = read(archive, &content_byte, 1);
                write_status = write(file, &content_byte, 1);

                // Handling errors
                if (read_status != 1 || write_status != 1) {
                    printf("[ERROR] Couldn't write file content from %s.\n", archive_path);

                    // Closing writing file
                    if (close(file) == -1) {
                        printf("[ERROR] Couldn't close %s.\n", path);
                    }

                    // Closing archive
                    if (close(archive) == -1) {
                        printf("[ERROR] Couldn't close %s.\n", archive_path);
                    }

                    // Free memory for path
                    free(path);

                    // exit from program
                    exit(1);
                }
            }

            // Echo packing status
            printf("[UNPACKED] %s\n", path);

            // Closing file descriptor
            if (close(file) == -1) {
                printf("[ERROR] Couldn't close %s.\n", path);
                free(path);
                break;
            }
        }

        if ((path_type)type == FOLDER_NAME) {
            // Create new directory
            if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
                printf("[ERROR] Couldn't create %s.\n", path);
                free(path);
                break;
            }
        }

        // Free buffer for path
        free(path);

        error = 0;
    }

    // Closing archive
    if (close(archive) == -1) {
        printf("[ERROR] Couldn't close %s.\n", archive_path);
        exit(0);
    }

    // Exit if error
    if (error) {
        exit(1);
    }

    // New line for pretty printing
    printf("\n");
}

char* _rename_root(char* path, u64 old_root_len, char* new_root) {
    // If new root is empty - return source path
    if (strcmp(new_root, "") == 0) {
        return path;
    }

    u64 trimmed_path_len = strlen(path) - old_root_len;
    u64 new_root_len = strlen(new_root);

    // Allocate memory for new path
    char* renamed = (char*)malloc(new_root_len + trimmed_path_len + 2);

    // If error return NULL for to clear memory in parent before exiting
    if (renamed == NULL) {
        free(path);
        return NULL;
    }

    // Place new root + '/' to start of renamed path
    memcpy(renamed, new_root, new_root_len);
    renamed[new_root_len++] = '/';

    // Append to renamed path trimmed real path
    memcpy(renamed + new_root_len, path + old_root_len, trimmed_path_len);
    renamed[new_root_len + trimmed_path_len] = '\0';

    // Remove extra '/' from renamed path
    _remove_extra_slash(renamed);

    // Free old path
    free(path);

    // Return renamed path
    return renamed;
}

int _getch() {
    int character;
    struct termios old_attr, new_attr;

    // Backup terminal attributes
    if (tcgetattr(STDIN_FILENO, &old_attr ) == -1) {
        printf("[ERROR] Couldn't get terminal attributes\n");
        exit(1);
    }

    // Disable echo
    new_attr = old_attr;
    new_attr.c_lflag &= ~(ICANON | ECHO );
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) == -1) {
        printf("[ERROR] Couldn't set terminal attributes\n");
        exit(1);
    }

    // Get input character
    character = getchar();
    if (character == EOF) {
        printf("[ERROR] Couldn't get user input.\n");
        return 1;
    }

    // Restore terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr) == -1) {
        printf("[ERROR] Couldn't reset terminal attributes.\n");
        exit(1);
    }

    return character;
}

void _remove_extra_slash(char* path) {
    u64 i = 0;
    u64 renamed_len = strlen(path);

    while (i < renamed_len) {
        if((path[i] == '/') && (path[i + 1] == '/')) {
            // If you find two slashes, shift all
            // subsequent characters to the left
            for(u64 k = i + 1; k < renamed_len; k++) {
                path[k] = path[k + 1];
            }

            // Decrease length
            renamed_len -= 1;
        }
        else {
            // Go to next character
            i += 1;
        }
    }
}