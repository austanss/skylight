#pragma once
#include <stdint.h>
#include <stddef.h>

typedef enum {
    VFS_STATUS_SUCCESS                  =  0,
    VFS_STATUS_NOT_FOUND                = -1,
    VFS_STATUS_UNAUTHORIZED             = -2,
    VFS_STATUS_DUPLICATE                = -3,
    VFS_STATUS_LOCKED                   = -4,
    VFS_STATUS_FAILED                   = -5,
    VFS_STATUS_BAD_HANDLE               = -6
} vfs_status_t;

typedef uint8_t vfs_handle_flags_t;
typedef uint8_t vfs_permission_flags_t;
typedef uint8_t vfs_volume_flags_t;
typedef uint8_t vfs_open_flags_t;

typedef uint8_t vfs_extras_t[23];

typedef enum {
    VFS_HANDLE_FLAG_WRITING             = 0x01,
    VFS_HANDLE_FLAG_EXECUTING           = 0x02,
    VFS_HANDLE_FLAG_SUPERUSER           = 0x04,
    VFS_HANDLE_FLAG_UNBUFFERED          = 0x08,
    VFS_HANDLE_FLAG_SYSTEM              = 0x10
} vfs_handle_flag_t;

typedef enum {
    VFS_PERMISSION_FLAG_WRITABLE        = 0x01,
    VFS_PERMISSION_FLAG_EXECUTABLE      = 0x02,
    VFS_PERMISSION_FLAG_LOCKED          = 0x04,
    VFS_PERMISSION_FLAG_TRANSPARENT     = 0x08,
    VFS_PERMISSION_FLAG_SYSTEM          = 0x10
} vfs_permission_flag_t;

typedef enum {
    VFS_VOLUME_FLAG_SYSTEM              = 0x01,
    VFS_VOLUME_FLAG_BOOT                = 0x02,
    VFS_VOLUME_FLAG_CACHE               = 0x04,
    VFS_VOLUME_FLAG_UNDISKED            = 0x08,
    VFS_VOLUME_FLAG_LOCKED              = 0x10,
    VFS_VOLUME_FLAG_UNBUFFERED          = 0x20
} vfs_volume_flag_t;

typedef enum {
    VFS_OPEN_FLAG_CREATE                = 0x01,
    VFS_OPEN_FLAG_TRUNCATE              = 0x02,
    VFS_OPEN_FLAG_WRITE                 = 0x04,
    VFS_OPEN_FLAG_UNBUFFERED            = 0x08
} vfs_open_flag_t;

typedef struct _vfs_handle vfs_handle_t;

typedef struct {
    vfs_status_t            (*fread)(vfs_handle_t* handle, void* buffer, size_t seek, size_t count);
    vfs_status_t            (*fwrite)(vfs_handle_t* handle, void* data, size_t seek, size_t count);
    vfs_status_t            (*fcreate)(vfs_handle_t* out, char* path, vfs_permission_flags_t permissions);
    vfs_status_t            (*fremove)(char* path);
    vfs_status_t            (*fopen)(vfs_handle_t* out, char* path, vfs_open_flags_t flags);
    vfs_status_t            (*fclose)(vfs_handle_t* handle);
} vfs_driver_t;

typedef struct {
    vfs_driver_t*           driver;
    unsigned char           identifier;
    vfs_volume_flags_t      flags;
    uint64_t                :48;
} vfs_volume_t;

struct _vfs_handle {
    char                    path[256];
    vfs_driver_t*           driver;
    vfs_volume_t*           volume;
    vfs_handle_flags_t      flags;
    vfs_extras_t            available;
};

// Path format: [Drive letter]::path/of/file.xxx
// Example: x::sys/start/frame.se
// Must be fully resolved file path, no .. or the like
vfs_status_t    vfs_open(vfs_handle_t* out, char* path, vfs_open_flags_t flags);
vfs_status_t    vfs_close(vfs_handle_t* handle);
vfs_status_t    vfs_read(vfs_handle_t* handle, void* buffer, size_t seek, size_t count);
vfs_status_t    vfs_write(vfs_handle_t* handle, void* data, size_t seek, size_t count);
vfs_status_t    vfs_create(vfs_handle_t* out, char* path, vfs_permission_flags_t flags);
vfs_status_t    vfs_delete(char* path);
