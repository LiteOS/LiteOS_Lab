/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#ifndef __VFS_H__
#define __VFS_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include <sys/fcntl.h>
#include <sys/stat.h>

#include <config.h>
#include <list.h>
#include <mutex.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* forward declaraton */

struct dirent;

/* typedefs */

struct f_ops
    {
    int (* open)         (uintptr_t, uintptr_t *, const char *, int);
    int (* close)        (uintptr_t);
    int (* read)         (uintptr_t, char *, size_t);
    int (* write)        (uintptr_t, const char *, size_t);
    int (* lseek)        (uintptr_t, int, int);
    int (* ioctl)        (uintptr_t, int, va_list);
    int (* sync)         (uintptr_t);
    };

struct m_ops
    {
    int (* stat)         (uintptr_t, const char *, struct stat *);
    int (* unlink)       (uintptr_t, const char *);
    int (* rename)       (uintptr_t, const char *, const char *);
    int (* opendir)      (uintptr_t, uintptr_t *, const char *);
    int (* readdir)      (uintptr_t, uintptr_t, struct dirent *);
    int (* closedir)     (uintptr_t, uintptr_t);
    int (* mkdir)        (uintptr_t, const char *);
    int (* mount)        (uintptr_t *, va_list);
    int (* unmount)      (uintptr_t);
    };

struct file_system
    {
    const char         * fs_name;

    const struct f_ops * fs_fops;
    const struct m_ops * fs_mops;

    dlist_t              fs_node;
    volatile uint32_t    fs_refs;
    };

struct mount_point
    {
    struct file_system * mp_fs;
    char               * mp_path;
    dlist_t              mp_node;
    mutex_t              mp_lock;
    volatile uint32_t    mp_refs;
    uintptr_t            mp_data;
    };

struct file
    {
    const struct f_ops * fl_fops;
    int                  fl_flags;
    struct mount_point * fl_mp;
    volatile uint32_t    fl_refs;
    uintptr_t            fl_data;
    char               * fl_path;
    };

/* file table entry */

struct fd
    {
    struct file        * fd_file;
    int                  fd_flags;
    bool                 fd_used;
    };

struct dirent
    {
    char                 de_name [CONFIG_MAX_FILE_NAME_LEN];
    const char         * de_type;
    size_t               de_size;
    };

struct dir
    {
    struct mount_point * dr_mp;
    struct dirent        dr_dent;
    uintptr_t            dr_data;
    };

/* externs */

extern int               vfs_open          (const char *, int);
extern int               vfs_close         (int);
extern int               vfs_read          (int, char *, size_t);
extern int               vfs_write         (int, const char *, size_t);
extern int               vfs_lseek         (int, int, int);
extern int               vfs_ioctl         (int, int, ...);
extern int               vfs_sync          (int);
extern int               vfs_stat          (const char *, struct stat *);
extern int               vfs_unlink        (const char *);
extern int               vfs_rename        (const char *, const char *);
extern struct dir      * vfs_opendir       (const char * o_path);
extern struct dirent   * vfs_readdir       (struct dir * dir);
extern int               vfs_closedir      (struct dir *);
extern int               vfs_mkdir         (const char *, int);
extern int               vfs_mount         (const char *, const char *, ...);
extern int               vfs_unmount       (const char *);
extern int               vfs_fs_register   (struct file_system * fs);
extern int               vfs_fs_unregister (struct file_system *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VFS_H__ */
