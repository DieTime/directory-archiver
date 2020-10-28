#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

#include "../include/archiver.h"

void pack(char* dir_path, char* out_path) {
    // Create output archive
    int arch = open(out_path, O_CREAT | O_WRONLY, S_IWUSR);
    if (arch == -1) {
        printf("[ERROR] Couldn't create %s output file.\n", out_path);
        exit(1);
    }

    if (write(arch, MAGIC, MAGIC_LENGTH) != MAGIC_LENGTH) {
        printf("[ERROR] Couldn't write magic header to %s output file.\n", out_path);
        exit(1);
    }

    _pack(arch, dir_path);
    printf("\n");

    // Close output archive
    if (close(arch) == -1) {
        printf("[ERROR] Couldn't close %s output file.\n", out_path);
        exit(1);
    }
}

void _pack(int arch, char* dir_path) {
    // Open directory by path
    DIR *dir = opendir(dir_path);

    // If directory path turned out to be a file path
    if (dir == NULL) {
        // Write info about file to archive
        _write_info(arch, FILE_NAME, dir_path);

        // Write file content to archive
        _write_content(arch, dir_path);

        // Echo packing status
        printf("%s <- Successfully writen\n", dir_path);

        return;
    }

    // Write info about folder to archive
    _write_info(arch, FOLDER_NAME, dir_path);

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
        snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // Pack entry files recursively if the entry is directory
            _pack(arch, file_path);
        }
        else {
            // Write info about file to archive
            _write_info(arch, FILE_NAME, file_path);

            // Write file content to archive
            _write_content(arch, file_path);

            // Echo packing status
            printf("%s <- Successfully writen\n", file_path);
        }
    }

    // Close directory
    if (closedir(dir) == -1) {
        printf("[ERROR] Couldn't close %s directory.\n", dir_path);
        exit(1);
    }
}

void _write_info(int fd, name_type file_type, char* file_path) {
    // Convert to uint8_t filetype
    u8 type = (u8)file_type;

    // Write to archive path type (file or folder)
    if (write(fd, &type, u8sz) != u8sz) {
        printf("[ERROR] Couldn't write file type to archive.\n");
        close(fd);
        exit(1);
    }

    // Write to archive file path length (file or folder)
    u64 dir_path_len = (u64)strlen(file_path);
    if (write(fd, &dir_path_len, u64sz) != u64sz) {
        printf("[ERROR] Couldn't write path length to archive.\n");
        close(fd);
        exit(1);
    }

    // Write to archive file path (file or folder)
    if (write(fd, file_path, dir_path_len) != dir_path_len) {
        printf("[ERROR] Couldn't write path to archive.\n");
        close(fd);
        exit(1);
    }
}

void _write_content(int fd, char* file_path) {
    // Open source file
    int source = open(file_path, O_RDONLY, S_IRUSR);
    if (source == -1) {
        printf("[ERROR] Couldn't open %s for reading.\n", file_path);
        close(fd);
        exit(1);
    }

    // Get size of file
    off_t size = lseek(source, 0, SEEK_END);
    if (size == -1) {
        printf("[ERROR] Couldn't get %s length.\n", file_path);
        close(fd);
        close(source);
        exit(1);
    }
    u64 file_size = (u64)(size + 1);

    // Seek back to the beginning
    if (lseek(source, 0, SEEK_SET) == -1) {
        printf("[ERROR] Couldn't set cursor in %s to start.\n", file_path);
        close(fd);
        close(source);
        exit(1);
    }

    // Write content size to archive
    if (write(fd, &file_size, u64sz) != u64sz) {
        printf("[ERROR] Couldn't write size of %s to archive.\n", file_path);
        close(fd);
        close(source);
        exit(1);
    }

    // Create buffer for content
    char *buffer = (char*)malloc(file_size);
    if (buffer == NULL) {
        printf("[ERROR] Couldn't set cursor in %s to start.\n", file_path);
        close(fd);
        close(source);
        exit(1);
    }

    // Read content of source file
    if (read(source, buffer, (file_size - 1)) != file_size - 1) {
        printf("[ERROR] Couldn't read content of %s.\n", file_path);
        close(fd);
        close(source);
        free(buffer);
        exit(1);
    }
    buffer[file_size - 1] = '\0';

    // Write content of source to archive
    if (write(fd, buffer, file_size) != file_size) {
        printf("[ERROR] Couldn't write content of %s to archive.\n", file_path);
        close(fd);
        close(source);
        free(buffer);
        exit(1);
    }

    // Close source file
    if (close(source) == -1) {
        printf("[ERROR] Couldn't close %s source file.\n", file_path);
        exit(1);
    }

    // Free memory for buffer
    free(buffer);
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