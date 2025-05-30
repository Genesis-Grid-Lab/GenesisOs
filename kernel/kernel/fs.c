#include "kernel/fs.h"
#include "kernel/heap.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static FileNode root = { "/", DIR_TYPE, NULL, {NULL}, 0, (uint8_t)NULL, true, (uint8_t)NULL };
static FileNode* current_directory = &root;

// Create file or directory
FileNode* fs_create(const char* name, NodeType type) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (!current_directory->children[i]) {
      FileNode* new_node = (FileNode*)malloc(sizeof(FileNode));
      strncpy(new_node->name, name, MAX_NAME_LEN);
      new_node->type = type;
      new_node->parent = current_directory;
      new_node->size = 0;
      new_node->used = true;
      new_node->execute = NULL;
      new_node->permissions = 7; // Full RWX by default
      memset(new_node->data, 0, MAX_FILE_SIZE);
      current_directory->children[i] = new_node;
      return new_node;
    }
  }
  return NULL;
}

// Change directory
uint8_t fs_cd(const char* name) {
    if (strcmp(name, "..") == 0) {
        if (current_directory->parent) {
            current_directory = current_directory->parent;
            return true;
        }
        return false;
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            if (current_directory->children[i]->type == DIR_TYPE) {
                current_directory = current_directory->children[i];
                return true;
            }
        }
    }
    return false;
}

// List files
void fs_ls() {
   printf("Contents of %s:\n", current_directory->name);
   for (int i = 0; i < MAX_FILES; i++) {
     if (current_directory->children[i]) {
       printf(" - %s %s [%c%c%c]\n", current_directory->children[i]->name,
	      current_directory->children[i]->type == DIR_TYPE ? "(DIR)" : "(FILE)",
	      (current_directory->children[i]->permissions & 4) ? 'r' : '-',
	      (current_directory->children[i]->permissions & 2) ? 'w' : '-',
	      (current_directory->children[i]->permissions & 1) ? 'x' : '-');
     }
   }
}

void fs_ls_node(const char* dir){

  FileNode *node;
  for(int i = 0; i < MAX_FILES; i++){
    if(current_directory->children[i] && strcmp(current_directory->children[i]->name, dir) == 0){
      node =current_directory;
    }
  }
  
  printf("Contents of %s:\n", node->name);
  for(int i = 0; i < MAX_FILES; i++){
    if(node->children[i]){
      printf(" - %s %s [%c%c%c]\n", node->children[i]->name,
	     node->children[i]->type == DIR_TYPE ? "(DIR)" : "(FILE)",
	     (node->children[i]->permissions & 4) ? 'r' : '-',
	     (node->children[i]->permissions & 2) ? 'w' : '-',
	     (node->children[i]->permissions & 1) ? 'x' : '-');
    }
  }
}

// Write data to a file
uint8_t fs_write(const char* name, const char* data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            if (current_directory->children[i]->type == FILE_TYPE) {
                int len = strlen(data);
                if (len > MAX_FILE_SIZE) len = MAX_FILE_SIZE;
                memcpy(current_directory->children[i]->data, data, len);
                current_directory->children[i]->size = len;
                return true;
            }
        }
    }
    return false;
}

// Read a file
const char* fs_read(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            return (const char*)current_directory->children[i]->data;
        }
    }
    return NULL;
}

// Register an executable
uint8_t fs_register_executable(const char* name, void (*func)(int, char**)) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            current_directory->children[i]->execute = func;
            return true;
        }
    }
    return false;
}

// Execute a program
uint8_t fs_exec(const char* name, int argc, char* argv[]) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            if (current_directory->children[i]->execute) {
                current_directory->children[i]->execute(argc, argv);
                return true;
            }
        }
    }
    return false;
}

// Change file permissions
uint8_t fs_chmod(const char* name, uint8_t permissions) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            current_directory->children[i]->permissions = permissions;
            return true;
        }
    }
    return false;
}

// Copy a file
uint8_t fs_copy(const char* src, const char* dest) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (current_directory->children[i] && strcmp(current_directory->children[i]->name, src) == 0) {
      FileNode* copy = fs_create(dest, FILE_TYPE);
      if (copy) {
	memcpy(copy->data, current_directory->children[i]->data, current_directory->children[i]->size);
	copy->size = current_directory->children[i]->size;
	return true;
      }
    }
  }
  return false;
}

// Delete file
uint8_t fs_delete(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, name) == 0) {
            free(current_directory->children[i]);
            current_directory->children[i] = NULL;
            return true;
        }
    }
    return false;
}

// Rename or move a file
uint8_t fs_rename(const char* old_name, const char* new_name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_directory->children[i] && strcmp(current_directory->children[i]->name, old_name) == 0) {
            strncpy(current_directory->children[i]->name, new_name, MAX_NAME_LEN);
            return true;
        }
    }
    return false;
}
