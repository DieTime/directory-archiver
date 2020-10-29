#pragma once
#ifndef TAR_ARCHIVER_H
#define TAR_ARCHIVER_H

#include <stdint.h>

#define u64   uint64_t
#define u8    uint8_t
#define u64sz sizeof(uint64_t)
#define u8sz  sizeof(uint8_t)
#define MAGIC "...DENCHIK..ARCHIVER..."
#define MAGIC_LENGTH 24
#define MAX_PATH_LENGTH 1024

enum _path_type {
    FILE_NAME = 0,
    FOLDER_NAME = 1,
};
typedef enum _path_type path_type;

void pack(char* dir_path, char* archive_path);
void _pack(int archive, char* src_path);
void _pack_info(int fd, path_type p_type, char* path);
void _pack_content(int fd, char* path);

void unpack(char* archive_path, char* out_path);
char* _rename_root(char* path, u64 old_root_len, char* new_root);

int  _getch();
void _remove_extra_slash(char* path);

#endif //TAR_ARCHIVER_H
