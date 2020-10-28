#pragma once
#ifndef TAR_ARCHIVER_H
#define TAR_ARCHIVER_H

#define u64 uint64_t
#define u8  uint8_t
#define u64sz sizeof(uint64_t)
#define u8sz  sizeof(uint8_t)

typedef enum _type_name {
    FILE_NAME = 0,
    FOLDER_NAME = 1,
} type_name;

void pack(char* dir_path, char* out_path);
void _pack(int arch, char* dir_path);
void _write_info(int fd, type_name file_type, char* file_path);
void _write_content(int fd, char* file_path);
int  _getch();

#endif //TAR_ARCHIVER_H
