#pragma once
#ifndef TAR_ARCHIVER_H
#define TAR_ARCHIVER_H

#define u64   uint64_t
#define u8    uint8_t
#define u64sz sizeof(uint64_t)
#define u8sz  sizeof(uint8_t)
#define MAGIC "...DENCHIK..ARCHIVER..."
#define MAGIC_LENGTH 24

enum _name_type {
    FILE_NAME = 0,
    FOLDER_NAME = 1,
};
typedef enum _name_type name_type;

void pack(char* dir_path, char* out_path);
void _pack(int arch, char* dir_path);
void _write_info(int fd, name_type file_type, char* file_path);
void _write_content(int fd, char* file_path);
int  _getch();

#endif //TAR_ARCHIVER_H
