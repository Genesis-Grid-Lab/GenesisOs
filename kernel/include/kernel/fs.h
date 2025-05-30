#ifndef _FS_H_
#define _FS_H_

#include <stdint.h>

#define MAX_FILES 16       // Maximum files per directory
#define MAX_FILE_SIZE 1024 // Max size per file
#define MAX_NAME_LEN 20    // Max filename length
#define MAX_DIR_DEPTH 10   // Max directory nesting

typedef enum { FILE_TYPE, DIR_TYPE } NodeType;

// File/directory structure
typedef struct FileNode {
    char name[MAX_NAME_LEN];
    NodeType type;
    struct FileNode* parent;
    struct FileNode* children[MAX_FILES];
    uint16_t size;
    uint8_t data[MAX_FILE_SIZE];
  uint8_t used;//bool
  uint8_t permissions; // RWX (Read = 4, Write = 2, Exec = 1)
    void (*execute)(int, char**); // Function pointer for executables
} FileNode;

FileNode *fs_create(const char *name, NodeType type);
uint8_t fs_cd(const char *name); // bool
void fs_ls(void);
void fs_ls_node(const char* dir);
uint8_t fs_delete(const char *name); // bool
uint8_t fs_rename(const char *old_name, const char *new_name); // bool
uint8_t fs_copy(const char *src, const char *dest);            // bool
uint8_t fs_chmod(const char* name, uint8_t permissions);// bool
uint8_t fs_exec(const char *name, int argc, char *argv[]); // bool
uint8_t fs_register_executable(const char* name, void (*func)(int, char**)); // bool
uint8_t fs_write(const char *name, const char *data);
const char *fs_read(const char *name);
void fs_list(void);

#endif
