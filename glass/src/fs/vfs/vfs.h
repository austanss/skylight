#pragma once
#include <stdint.h>
#include <stddef.h>

#define VFS_MAX_FILENAME_LENGTH 256

#define VFS_STATUS_SUCCESS                   0
#define VFS_STATUS_FILE_NOT_FOUND           -1
#define VFS_STATUS_IS_DIRECTORY             -2
#define VFS_STATUS_BOUND_RANGE_EXCEEDED     -3
#define VFS_STATUS_NAME_TOO_LONG            -4
#define VFS_STATUS_INVALID_PERMISSIONS      -5

typedef struct {
    int         (*fread)(vfs_node_t* target, size_t position, size_t bytes, void* buffer);
    int         (*fwrite)(vfs_node_t* target, void* data, size_t position, size_t bytes);
    int         (*fcreate)(vfs_node_t* out, char* name, uint8_t permissions);
    int         (*fdelete)(vfs_node_t* target);
} vfs_driver_t;

typedef struct _vfs_node vfs_node_t;

typedef struct {
    unsigned char   label;
    vfs_driver_t*   driver;
    vfs_node_t*     files;
} vfs_volume_t;

typedef struct {
    bool    executable  :1;
    bool    writable    :1;
    bool    owner       :1;
    bool    system      :1;
    bool    hidden      :1;
    bool    open        :1;
    bool                :2;
} vfs_permissions_t;

struct _vfs_node {
    char                identifier[VFS_MAX_FILENAME_LENGTH];
    vfs_volume_t*       volume;
    vfs_node_t*         parent;
    vfs_node_t*         children;
    vfs_node_t*         next;
    vfs_permissions_t   permissions;
    uint8_t             available[16];
};
