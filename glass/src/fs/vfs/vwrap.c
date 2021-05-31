#include "vfs.h"

vfs_status_t vclose(vfs_handle_t* handle) {
    if (!handle || !handle->driver)
        return VFS_STATUS_BAD_HANDLE;

    return handle->driver->fclose(handle);
}

vfs_status_t vread(vfs_handle_t* handle, void* buffer, size_t seek, size_t count) {
    if (!handle || !handle->driver)
        return VFS_STATUS_BAD_HANDLE;

    return handle->driver->fread(handle, buffer, seek, count);
}

vfs_status_t vwrite(vfs_handle_t* handle, void* data, size_t seek, size_t count) {
    if (!handle || !handle->driver)
        return VFS_STATUS_BAD_HANDLE;

    return handle->driver->fwrite(handle, data, seek, count);
}
