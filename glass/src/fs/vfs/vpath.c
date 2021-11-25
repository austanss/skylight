#include "vfs.h"
#include <stdbool.h>

static vfs_volume_t volumes[256];

static void _volumes_initialize() {
    for (size_t i = 0; i < 256; i++) {
        volumes[i].identifier = '\0';
        volumes[i].driver = NULL;
        volumes[i].flags = 0;
    }
}

static bool _volumes_clean = false;

vfs_status_t vcreate(vfs_handle_t* out, char* path, vfs_permission_flags_t flags) {
    if (!_volumes_clean)
        _volumes_initialize();
    
    unsigned char index = (unsigned char)path[0];

    if (path[1] != ':')
        return VFS_STATUS_NOT_FOUND;

    path += 2;

    vfs_volume_t* volume = &volumes[index];
    
    if (!volume->identifier)
        return VFS_STATUS_NOT_FOUND;

    return volume->driver->fcreate(out, path, flags);
}

vfs_status_t vdelete(char* path) {
    if (!_volumes_clean)
        _volumes_initialize();
    
    unsigned char index = (unsigned char)path[0];

    if (path[1] != ':')
        return VFS_STATUS_NOT_FOUND;

    path += 2;

    vfs_volume_t* volume = &volumes[index];
    
    if (!volume->identifier)
        return VFS_STATUS_NOT_FOUND;

    return volume->driver->fremove(path);
}

vfs_status_t vopen(vfs_handle_t* out, char* path, vfs_open_flags_t flags) {
    if (!_volumes_clean)
        _volumes_initialize();
    
    unsigned char index = (unsigned char)path[0];

    if (path[1] != ':')
        return VFS_STATUS_NOT_FOUND;

    path += 2;

    vfs_volume_t* volume = &volumes[index];
    
    if (!volume->identifier)
        return VFS_STATUS_NOT_FOUND;

    // 0xffffffff8140d804
    return volume->driver->fopen(out, path, flags);
}
