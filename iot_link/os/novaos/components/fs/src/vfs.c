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

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <sys/stat.h>

#include <config.h>
#include <common.h>
#include <mutex.h>
#include <list.h>
#include <init.h>
#include <warn.h>
#include <bug.h>
#include <vfs.h>

#ifdef CONFIG_CMDER
#include <stdio.h>
#include <cmder.h>
#endif

/* locals */

static dlist_t fs_head = DLIST_INIT (fs_head);
static dlist_t mp_head = DLIST_INIT (mp_head);
static mutex_t fs_lock;

static struct fd fd_table [CONFIG_MAX_NR_FILES] = {0};

/*
 * get mount_point struct by using name, note: this routine must be invoked with
 * the fs_lock locked
 *
 * note: there is no '//' in <path>, see __get_new_path
 */

static struct mount_point * __path_resolve (const char  * path,
                                            const char ** rela_path)
    {
    dlist_t            * itr;
    struct mount_point * best     = NULL;
    size_t               best_len = 0;

    dlist_foreach (itr, &mp_head)
        {
        size_t               len;       /* mount point path string length */
        struct mount_point * mp;
        const char         * sub;

        mp = container_of (itr, struct mount_point, mp_node);

        sub = strstr (path, mp->mp_path);

        if ((sub == NULL) || sub != path)
            {
            continue;
            }

        len = strlen (mp->mp_path);

        if (path [len] == '\0')
            {
            best_len = len - 1;             /* so, best_len + 1 == len */
            best     = mp;
            break;
            }

        if ((len > best_len) && (path [len] == '/'))
            {
            best_len = len;
            best     = mp;
            }
        }

    if (rela_path != NULL)
        {
        *rela_path = path + best_len + 1;   /* plus 1 to skip the '/' */
        }

    return best;
    }

static int __get_fildes (const char * path)
    {
    int i;

    for (i= 0; i < CONFIG_MAX_NR_FILES; i++)
        {
        if (!fd_table [i].fd_used)
            {
            fd_table [i].fd_used = true;

            return i;
            }
        }

    return -1;
    }

static struct file * __get_opened_file (const char * path)
    {
    int i;

    for (i= 0; i < CONFIG_MAX_NR_FILES; i++)
        {
        if ((fd_table [i].fd_file != NULL) &&
            (strcmp (fd_table [i].fd_file->fl_path, path) == 0))
            {
            return fd_table [i].fd_file;
            }
        }

    return NULL;
    }

/* check if there is any file opened under a path (dir) */

static bool __has_opened_file (const char * path)
    {
    int    i;
    size_t len = strlen (path);

    for (i= 0; i < CONFIG_MAX_NR_FILES; i++)
        {
        if ((fd_table [i].fd_file != NULL) &&
            (strncmp (fd_table [i].fd_file->fl_path, path, len) == 0))
            {
            return true;
            }
        }

    return false;
    }

static inline char * __get_new_path (const char * path)
    {
    char * p = (char *) malloc (strlen (path) + 1);
    char * t = p;
    char   ch;
    char   last = '\0';

    if (p == NULL)
        {
        return NULL;
        }

    while ((ch = *path++) != '\0')
        {
        if ((ch == '/') && (last == '/'))
            {
            continue;
            }

        last = ch;
        *t++ = ch;
        }

    /* remove the last '/' in path */

    if (last == '/')
        {
        t [-1] = '\0';
        }
    else
        {
        t [0]  = '\0';
        }

    return p;
    }

/**
 * vfs_open - virtual filesystem switch open method
 * @path:  the file path
 * @flags: the open flags
 *
 * return : a positive integer as a fd on success, -1 on error
 */

int vfs_open (const char * o_path, int o_flags)
    {
    int                  fd = -1;
    const char         * rela_path;
    struct file        * file;
    struct mount_point * mp;
    struct stat          stat;
    char               * path;
    bool                 fs_locked = false;

    /* can not open dir */

    if ((o_path == NULL) || (o_path [strlen (o_path) - 1] == '/'))
        {
        errno = EINVAL;
        return -1;
        }

    path = __get_new_path (o_path);

    if (path == NULL)
        {
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        goto out_free_path;
        }

    fs_locked = true;

    if ((fd = __get_fildes (path)) == -1)
        {
        errno = ENFILE;
        goto out_unlock_fs;
        }

    if ((o_flags & O_EXCL) && (__get_opened_file (path) != NULL))
        {
        errno = EACCES;
        goto out_release_fd;
        }

    if ((mp = __path_resolve ((const char *) path, &rela_path)) == NULL)
        {
        errno = ENOENT;
        goto out_release_fd;
        }

    if (*rela_path == '\0')
        {
        errno = EINVAL;
        goto out_release_fd;
        }

    /* lock mount point to prevent file deleted */

    if (mutex_lock (&mp->mp_lock) != 0)
        {
        errno = EAGAIN;
        goto out_release_fd;
        }

    if (mp->mp_fs->fs_mops->stat (mp->mp_data, rela_path, &stat) != 0)
        {
        if ((o_flags & O_CREAT) == 0)
            {
            errno = ENOENT;
            goto out_unlock_mp;
            }

        /* file not exist, will create it, set stat.st_mode to 777 */

        stat.st_mode = 0666;
        }
    else
        {
        if (__stat_is_dir (stat.st_mode))
            {
            errno = EINVAL;
            goto out_unlock_mp;
            }

        if (((stat.st_mode & S_IRUSR) == 0) && (o_flags & O_ACCMODE) != O_WRONLY)
            {
            errno = EACCES;
            goto out_unlock_mp;
            }

        if (((stat.st_mode & S_IWUSR) == 0) && (o_flags & O_ACCMODE) != O_RDONLY)
            {
            errno = EACCES;
            goto out_unlock_mp;
            }

        /* only files like a stream can reuse the file struct (no offset) */

        if (!__stat_is_blk (stat.st_mode) && !__stat_is_reg (stat.st_mode) &&
            (file = __get_opened_file (path)) != NULL)
            {
            file->fl_refs++;

            fd_table [fd].fd_flags = o_flags;
            fd_table [fd].fd_file  = file;

            (void) mutex_unlock (&fs_lock);

            (void) mutex_unlock (&mp->mp_lock);

            free (path);

            return fd;
            }
        }

    (void) mutex_unlock (&fs_lock);

    fs_locked = false;

    file = (struct file *) malloc (sizeof (struct file));

    if (file == NULL)
        {
        goto out_unlock_mp;
        }

    file->fl_path  = path;
    file->fl_flags = o_flags & ~(O_ACCMODE | O_CREAT | O_TRUNC | O_APPEND);
    file->fl_fops  = mp->mp_fs->fs_fops;
    file->fl_mp    = mp;
    file->fl_refs  = 1;
    file->fl_data  = 0;

    switch (stat.st_mode & (S_IRUSR | S_IWUSR))
        {
        case S_IRUSR | S_IWUSR:
            file->fl_flags |= O_RDWR;
            break;
        case S_IRUSR:
            file->fl_flags |= O_RDONLY;
            break;
        case S_IWUSR:
            file->fl_flags |= O_WRONLY;
            break;
        default:
            goto err_out_free_file;
        }

    fd_table [fd].fd_flags = o_flags;

    o_flags &= (O_CREAT | O_TRUNC | O_APPEND);
    o_flags |= file->fl_flags;

    /* open file with the most right */

    if (file->fl_fops->open (mp->mp_data, &file->fl_data, rela_path, o_flags) != 0)
        {
        goto err_out_free_file;
        }

    fd_table [fd].fd_file  = file;

    mp->mp_refs++;

    (void) mutex_unlock (&mp->mp_lock);

    return fd;

err_out_free_file:
    free (file);
out_unlock_mp:
    (void) mutex_unlock (&mp->mp_lock);
out_release_fd:
    fd_table [fd].fd_used = false;
    fd = -1;
out_unlock_fs:
    if (fs_locked)
        {
        (void) mutex_unlock  (&fs_lock);
        }
out_free_path:
    free (path);

    return fd;
    }

/**
 * vfs_close - virtual filesystem switch close interface
 * @fd: the file descriptor to be closed
 *
 * return : 0 on success, -1 on error
 */

int vfs_close (int fd)
    {
    struct file        * file;
    struct mount_point * mp;
    int                  ret = -1;

    if (fd < 0 || fd >= CONFIG_MAX_NR_FILES)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    file = fd_table [fd].fd_file;

    if (file == NULL)
        {
        errno = EBADF;
        mutex_unlock (&fs_lock);
        return -1;
        }

    mp = file->fl_mp;

    if ((ret = mutex_lock (&mp->mp_lock)) != 0)
        {
        errno = EAGAIN;
        mutex_unlock (&fs_lock);
        return -1;
        }

    fd_table [fd].fd_file  = NULL;
    fd_table [fd].fd_used  = false;
    fd_table [fd].fd_flags = 0;

    mutex_unlock (&fs_lock);

    file->fl_refs--;

    if (likely (file->fl_refs == 0))
        {
        if (likely (file->fl_fops->close != NULL))
            {
            (void) file->fl_fops->close (file->fl_data);
            }

        mp->mp_refs--;

        free (file->fl_path);
        free (file);
        }

    /* ret already 0 */

    mutex_unlock (&mp->mp_lock);

    return 0;
    }

/**
 * vfs_close - virtual filesystem switch read interface
 * @fd:    the file descriptor
 * @buff:  the data buffer
 * @nbyte: the number of bytes
 *
 * return : number of read on success, -1 on error
 */

int vfs_read (int fd, char * buff, size_t nbyte)
    {
    struct file        * file;
    struct mount_point * mp;
    int                  ret = -1;

    if (fd < 0 || fd >= CONFIG_MAX_NR_FILES)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    if ((file = fd_table [fd].fd_file) == NULL)
        {
        errno = EBADF;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    if ((fd_table [fd].fd_flags & O_ACCMODE) == O_WRONLY)
        {
        errno = EACCES;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    mp  = file->fl_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (unlikely (ret != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    ret = file->fl_fops->read (file->fl_data, buff, nbyte);

    (void) mutex_unlock (&mp->mp_lock);

    return ret;
    }

/**
 * vfs_write - virtual filesystem switch write interface
 * @fd:    the file descriptor
 * @buff:  the data buffer
 * @nbyte: the number of bytes
 *
 * return : number of write on success, -1 on error
 */

int vfs_write (int fd, const char * buff, size_t nbyte)
    {
    struct file        * file;
    struct mount_point * mp;
    int                  ret = -1;

    if (fd < 0 || fd >= CONFIG_MAX_NR_FILES)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    if ((file = fd_table [fd].fd_file) == NULL)
        {
        errno = EBADF;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    if ((fd_table [fd].fd_flags & O_ACCMODE) == O_RDONLY)
        {
        errno = EACCES;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    mp  = file->fl_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (unlikely (ret != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    ret = file->fl_fops->write (file->fl_data, buff, nbyte);

    (void) mutex_unlock (&mp->mp_lock);

    return ret;
    }

/**
 * vfs_lseek - virtual filesystem switch lseek interface
 * @fd:    the file descriptor
 * @offset: offset bytes
 * @whence: set, cur or end
 *
 * return : new offset on success, -1 on error
 */

int vfs_lseek (int fd, int offset, int whence)
    {
    struct file        * file;
    struct mount_point * mp;
    int                  ret = -1;

    if (fd < 0 || fd >= CONFIG_MAX_NR_FILES)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    if ((file = fd_table [fd].fd_file) == NULL)
        {
        errno = EBADF;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    if (file->fl_fops->lseek == NULL)
        {
        errno = ENOTSUP;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    mp  = file->fl_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (unlikely (ret != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    ret = file->fl_fops->lseek (file->fl_data, offset, whence);

    (void) mutex_unlock (&mp->mp_lock);

    return ret;
    }

/**
 * vfs_ioctl - virtual filesystem switch ioctl interface
 * @fd:  the file path
 * @req: the output stat
 *
 * return : 0 on success, -1 on error
 */

int vfs_ioctl (int fd, int req, ...)
    {
    struct file        * file;
    struct mount_point * mp;
    int                  ret = -1;
    va_list              valist;

    if (fd < 0 || fd >= CONFIG_MAX_NR_FILES)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    if ((file = fd_table [fd].fd_file) == NULL)
        {
        errno = EBADF;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    if (file->fl_fops->ioctl == NULL)
        {
        errno = ENOTSUP;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    mp  = file->fl_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (unlikely (ret != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    va_start (valist, req);
    ret = file->fl_fops->ioctl (file->fl_data, req, valist);
    va_end (valist);

    (void) mutex_unlock (&mp->mp_lock);

    return ret;
    }

/**
 * vfs_sync - virtual filesystem switch sync interface
 * @fd: the file descriptor
 *
 * return : 0 on success, -1 on error
 */

int vfs_sync (int fd)
    {
    struct file        * file;
    struct mount_point * mp;
    int                  ret = -1;

    if (fd < 0 || fd >= CONFIG_MAX_NR_FILES)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    if ((file = fd_table [fd].fd_file) == NULL)
        {
        errno = EBADF;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    if (file->fl_fops->sync == NULL)
        {
        errno = ENOTSUP;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    /* sync need write right */

    if ((fd_table [fd].fd_flags & O_ACCMODE) == O_RDONLY)
        {
        errno = EACCES;
        (void) mutex_unlock (&fs_lock);
        return -1;
        }

    mp  = file->fl_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (unlikely (ret != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    ret = file->fl_fops->sync (file->fl_data);

    (void) mutex_unlock (&mp->mp_lock);

    return ret;
    }

/**
 * vfs_stat - virtual filesystem switch stat interface
 * @path: the file path
 * @stat: the output stat
 *
 * return : 0 on success, -1 on error
 */

int vfs_stat (const char * s_path, struct stat * stat)
    {
    struct mount_point * mp;
    const char         * rela_path;
    int                  ret = -1;
    char               * path = __get_new_path (s_path);

    if (path == NULL)
        {
        errno = EINVAL;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        goto out_free_path;
        }

    if ((mp = __path_resolve (path, &rela_path)) == NULL)
        {
        errno = ENOENT;
        }
    else
        {
        ret = mutex_lock (&mp->mp_lock);
        }

    (void) mutex_unlock (&fs_lock);

    if (likely (ret == 0))
        {

        /* fs_mops->stat never NULL */

        ret = mp->mp_fs->fs_mops->stat (mp->mp_data, rela_path, stat);
        (void) mutex_unlock (&mp->mp_lock);
        }

out_free_path:

    free (path);

    return ret;
    }

/**
 * vfs_unlink - virtual filesystem switch unlink interface
 * @path: the file path
 *
 * return : 0 on success, -1 on error
 */

int vfs_unlink (const char * u_path)
    {
    struct mount_point * mp;
    const char         * rela_path;
    int                  ret = -1;
    char               * path = __get_new_path (u_path);

    if (path == NULL)
        {
        errno = EINVAL;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        goto out_free_path;
        }

    /* file is opened */

    if (__get_opened_file (path) != NULL)
        {
        errno = ENFILE;
        goto out_unlock_fs;
        }

    /* if there is any file under the dir of path */

    if (__has_opened_file (path))
        {
        errno = EBUSY;
        goto out_unlock_fs;
        }

    if ((mp = __path_resolve (path, &rela_path)) == NULL)
        {
        errno = ENOENT;
        goto out_unlock_fs;
        }

    if (mp->mp_fs->fs_mops->unlink == NULL)
        {
        errno = ENOTSUP;
        goto out_unlock_fs;
        }

    if (mutex_lock (&mp->mp_lock) == 0)
        {
        (void) mutex_unlock (&fs_lock);
        ret = mp->mp_fs->fs_mops->unlink (mp->mp_data, rela_path);
        (void) mutex_unlock (&mp->mp_lock);

        free (path);

        return ret;
        }

out_unlock_fs:
    (void) mutex_unlock (&fs_lock);
out_free_path:
    free (path);

    return ret;
    }

/**
 * vfs_rename - virtual filesystem switch rename interface
 * @old: the old file path
 * @old: the new file path
 *
 * return : 0 on success, -1 on error
 */

int vfs_rename (const char * old, const char * new)
    {
    struct mount_point * mp;
    char               * old_path;
    char               * new_path;
    const char         * rela_path_old;
    const char         * rela_path_new;
    int                  ret = -1;

    if (old == NULL || new == NULL)
        {
        errno = EINVAL;
        return -1;
        }

    if ((old_path = __get_new_path (old)) == NULL)
        {
        return -1;
        }

    if ((new_path = __get_new_path (new)) == NULL)
        {
        goto out_free_old;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        goto out_free_new;
        }

    /* old or new is already opened file */

    if ((__get_opened_file (old_path) != NULL) ||
        (__get_opened_file (new_path) != NULL) ||
        (__has_opened_file (old_path)))
        {
        errno = EBUSY;
        goto out_unlock_fs;
        }

    if ((mp = __path_resolve ((const char *) old_path, &rela_path_old)) == NULL)
        {
        errno = ENOENT;
        goto out_unlock_fs;
        }

    if (mp != __path_resolve ((const char *)new_path, &rela_path_new))
        {
        errno = EXDEV;
        goto out_unlock_fs;
        }

    if (mp->mp_fs->fs_mops->rename == NULL)
        {
        errno = ENOTSUP;
        goto out_unlock_fs;
        }

    if (mutex_lock (&mp->mp_lock) == 0)
        {
        (void) mutex_unlock (&fs_lock);
        ret = mp->mp_fs->fs_mops->rename (mp->mp_data, rela_path_old, rela_path_new);
        (void) mutex_unlock (&mp->mp_lock);

        goto out_free_new;
        }

out_unlock_fs:
    (void) mutex_unlock (&fs_lock);
out_free_new:
    free (new_path);
out_free_old:
    free (old_path);

    return ret;
    }

/**
 * vfs_rename - virtual filesystem switch dir open interface
 * @path: the dir path
 *
 * return : dir object pointer on success, NULL on error
 */

struct dir * vfs_opendir (const char * o_path)
    {
    struct mount_point * mp;
    const char         * rela_path = NULL;
    struct dir         * dir = NULL;
    int                  ret = -1;
    char               * path;

    if (o_path == NULL)
        {
        errno = EINVAL;
        return NULL;
        }

    if ((path = __get_new_path (o_path)) == NULL)
        {
        return NULL;
        }

    if ((dir = (struct dir *) malloc (sizeof (struct dir))) == NULL)
        {
        goto out_free_path;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        goto out_free_dir;
        }

    if ((mp = __path_resolve (path, &rela_path)) == NULL)
        {
        errno = ENOENT;
        goto out_unlock_fs;
        }

    if (likely (mutex_lock (&mp->mp_lock) == 0))
        {
        (void) mutex_unlock (&fs_lock);

        if (unlikely (mp->mp_fs->fs_mops->opendir != NULL))
            {
            ret = mp->mp_fs->fs_mops->opendir (mp->mp_data, &dir->dr_data, rela_path);
            }
        else
            {
            errno = ENOTSUP;
            }

        (void) mutex_unlock (&mp->mp_lock);

        free (path);

        if (ret != 0)
            {
            goto out_free_dir;
            }

        dir->dr_mp = mp;

        mp->mp_refs++;

        return dir;
        }

    errno = EAGAIN;

out_unlock_fs:
    (void) mutex_unlock (&fs_lock);
out_free_dir:
    free (dir);
out_free_path:
    free (path);

    return NULL;
    }

/**
 * vfs_readdir - virtual filesystem switch dir read interface
 * @dir: the dir object
 *
 * return : dirent object pointer on success, NULL on error
 */

struct dirent * vfs_readdir (struct dir * dir)
    {
    struct mount_point * mp;
    struct dirent      * ent = NULL;
    int                  ret;

    if (dir == NULL)
        {
        errno = EBADF;
        return NULL;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return NULL;
        }

    mp  = dir->dr_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (ret != 0)
        {
        errno = EAGAIN;
        return NULL;
        }

    if (mp->mp_fs->fs_mops->readdir != NULL)
        {
        if (mp->mp_fs->fs_mops->readdir (mp->mp_data, dir->dr_data,
                                         &dir->dr_dent) == 0)
            {
            ent = &dir->dr_dent;
            }
        }
    else
        {
        errno = ENOTSUP;
        }

    (void) mutex_unlock (&mp->mp_lock);

    return ent;
    }

/**
 * vfs_readdir - virtual filesystem switch dir close interface
 * @dir: the dir object
 *
 * return : 0 on on success, -1 on error
 */

int vfs_closedir (struct dir * dir)
    {
    struct mount_point * mp;
    int                  ret;

    if (dir == NULL)
        {
        errno = EBADF;
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        return -1;
        }

    mp  = dir->dr_mp;

    ret = mutex_lock (&mp->mp_lock);

    (void) mutex_unlock (&fs_lock);

    if (unlikely (ret != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    if (mp->mp_fs->fs_mops->closedir != NULL)
        {
        (void) mp->mp_fs->fs_mops->closedir (mp->mp_data, dir->dr_data);
        }
    else
        {
        errno = ENOTSUP;
        }

    free (dir);

    mp->mp_refs--;

    (void) mutex_unlock (&mp->mp_lock);

    return 0;
    }

/**
 * vfs_readdir - virtual filesystem switch dir create interface
 * @dir: the dir object
 *
 * return : 0 on on success, -1 on error
 */

int vfs_mkdir (const char * c_path, int c_mode)
    {
    struct mount_point * mp;
    char               * path;
    const char         * rela_path;
    int                  ret = -1;

    if (c_path == NULL)
        {
        errno = EINVAL;
        return -1;
        }

    if ((path = __get_new_path (c_path)) == NULL)
        {
        return -1;
        }

    if (mutex_lock (&fs_lock) != 0)
        {
        errno = EAGAIN;
        goto out_free_path;
        }

    if ((mp = __path_resolve ((const char *) path, &rela_path)) == NULL)
        {
        errno = ENOENT;
        goto out_unlock_fs;
        }

    if (*rela_path == '\0')     /* file/dir existed */
        {
        errno = EEXIST;
        goto out_unlock_fs;
        }

    if (unlikely (mutex_lock (&mp->mp_lock) != 0))
        {
        errno = EAGAIN;
        goto out_unlock_fs;
        }

    (void) mutex_unlock (&fs_lock);

    if (unlikely (mp->mp_fs->fs_mops->mkdir != NULL))
        {
        ret = mp->mp_fs->fs_mops->mkdir (mp->mp_data, rela_path);
        }
    else
        {
        errno = ENOTSUP;
        }

    (void) mutex_unlock (&mp->mp_lock);

    free (path);

    return ret;

out_unlock_fs:
    (void) mutex_unlock (&fs_lock);
out_free_path:
    free (path);

    return ret;
    }

/*
 * get file_system struct by using name, note: this routine must be invoked with
 * the fs_lock locked
 */

static struct file_system * __get_fs_by_name (const char * name)
    {
    dlist_t            * itr;
    struct file_system * fs;

    dlist_foreach (itr, &fs_head)
        {
        fs = container_of (itr, struct file_system, fs_node);

        if (strcmp (name, fs->fs_name) == 0)
            {
            return fs;
            }
        }

    return NULL;
    }

/**
 * vfs_mount - virtual filesystem switch mount interface
 * @fs_name: the filesystem name
 * @path:    the mount point path name
 *
 * more variable is passed depend on the filesystem type
 *
 * return : 0 on success, negtive value on error
 */

int vfs_mount (const char * fs_name, const char * mp_path, ...)
    {
    int                  ret;
    struct file_system * fs;
    struct mount_point * mp;
    va_list              valist;
    char               * path;
    const char         * temp;

    if (fs_name == NULL || mp_path == NULL || mp_path [0] != '/')
        {
        errno = EINVAL;
        return -1;
        }

    path = __get_new_path (mp_path);

    if (path == NULL)
        {
        return -1;
        }

    /* there is no '/' in the end of mp_name, see __get_new_path */

    if ((ret = mutex_lock (&fs_lock)) != 0)
        {
        errno = EAGAIN;
        goto err_out_free_path;
        }

    if ((fs = __get_fs_by_name (fs_name)) == NULL)
        {
        errno = ENOENT;
        goto err_out_unlock_fs;
        }

    if ((__path_resolve ((const char *) path, &temp) != NULL) && (*temp == '\0'))
        {
        errno = EEXIST;
        goto err_out_unlock_fs;
        }

    mp = (struct mount_point*) malloc (sizeof (struct mount_point));

    mp->mp_path = path;
    mp->mp_fs   = fs;
    mp->mp_refs = 0;

    if (mutex_init (&mp->mp_lock) != 0)
        {
        goto err_out_free_mp;
        }

    va_start (valist, mp_path);

    ret = fs->fs_mops->mount (&mp->mp_data, valist);

    va_end   (valist);

    if (ret != 0)
        {
        goto err_out_free_mp;
        }

    dlist_add (&mp_head, &mp->mp_node);

    fs->fs_refs++;

    mutex_unlock (&fs_lock);

    return 0;

err_out_free_mp:
    free (mp);
err_out_unlock_fs:
    mutex_unlock (&fs_lock);
err_out_free_path:
    free (path);

    return -1;
    }

/**
 * vfs_unmount - virtual filesystem switch unmount interface
 * @path: the mount point path name
 *
 * return : 0 on success, negtive value on error
 */

int vfs_unmount (const char * path)
    {
    struct mount_point * mp;
    const char         * temp;

    if (unlikely (mutex_lock (&fs_lock) != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    mp = __path_resolve (path, &temp);

    if (unlikely ((mp == NULL) || (*temp != '\0')))
        {
        errno = ENOENT;
        mutex_unlock (&fs_lock);
        return -1;
        }

    if (unlikely (mp->mp_refs != 0))
        {
        errno = EBUSY;
        mutex_unlock (&fs_lock);
        return -1;
        }

    if (unlikely (mp->mp_fs->fs_mops->unmount == NULL))
        {
        errno = ENOTSUP;
        mutex_unlock (&fs_lock);
        return -1;
        }

    if (mp->mp_fs->fs_mops->unmount (mp->mp_data) != 0)
        {
        mutex_unlock (&fs_lock);
        return -1;
        }

    dlist_del (&mp->mp_node);

    mp->mp_fs->fs_refs--;

    free (mp->mp_path);
    free (mp);

    mutex_unlock (&fs_lock);

    return 0;
    }

static bool __is_fs_name_valid (const char * fs_name)
    {
    int          ch;
    const char * name = fs_name;

    while ((ch = *name++) != '\0')
        {
        if (!islower (ch | 0x20) && !isdigit (ch))
            {
            return false;
            }
        }

    return name != fs_name;
    }

/**
 * vfs_fs_register - register a filesystem to the virtual filesystem switch
 * @fs: the filesystem struct
 *
 * return : 0 on success, negtive value on error
 */

int vfs_fs_register (struct file_system * fs)
    {
    int ret;

    BUG_ON (fs                 == NULL ||
            fs->fs_name        == NULL ||
            fs->fs_fops        == NULL ||
            fs->fs_fops->open  == NULL ||
            fs->fs_fops->read  == NULL ||
            fs->fs_fops->write == NULL ||
            fs->fs_mops        == NULL ||
            fs->fs_mops->stat  == NULL ||
            fs->fs_mops->mount == NULL,
            "Invalid filesystem!");

    /* check fs name */

    BUG_ON (!__is_fs_name_valid (fs->fs_name), "Invalid filesystem name!");

    fs->fs_refs = 0;

    mutex_lock (&fs_lock);

    if (__get_fs_by_name (fs->fs_name) == NULL)
        {
        dlist_add (&fs_head, &fs->fs_node);
        ret = 0;
        }
    else
        {
        ret = -1;
        }

    mutex_unlock (&fs_lock);

    return ret;
    }

/**
 * vfs_fs_unregister - unregister a filesystem from the virtual filesystem switch
 * @fs: the filesystem struct
 *
 * return : 0 on success, negtive value on error
 */

int vfs_fs_unregister (struct file_system * fs)
    {
    int ret;

    if (unlikely (mutex_lock (&fs_lock) != 0))
        {
        errno = EAGAIN;
        return -1;
        }

    if (fs->fs_refs != 0)
        {
        WARN ("Filesystem is busy!");
        errno = EBUSY;
        ret = -1;
        }
    else
        {
        dlist_del (&fs->fs_node);
        ret = 0;
        }

    mutex_unlock (&fs_lock);

    return ret;
    }

/**
 * vfs_init - the virtual filesystem switch initialization routine
 *
 * return : 0 on success, negtive value on error
 */

static int vfs_init (void)
    {
    return mutex_init (&fs_lock);
    }

MODULE_INIT (kernel, vfs_init);

#ifdef CONFIG_CMDER
static char * __cwd = NULL;

static void __ls_root (cmder_t * cmder)
    {
    dlist_t    * itr;
    char       * pch;

    dlist_foreach (itr, &mp_head)
        {
        pch = container_of (itr, struct mount_point, mp_node)->mp_path;

        do
            {
            cmder_printf (cmder, "%c", *pch++);
            } while ((*pch != '/') && (*pch != '\0'));

        cmder_printf (cmder, "\n");
        }
    }

static int __cmd_ls (cmder_t * cmder, int argc, char * argv [])
    {
    struct dir    * dir;
    struct dirent * ent;

    if (argc == 1)
        {
        if (__cwd == NULL)
            {
            __ls_root (cmder);
            return 0;
            }

        dir = vfs_opendir (__cwd);
        }
    else
        {
        if (argv [1][0] == '/')
            {
            if (argv [1][1] == '\0')
                {
                __ls_root (cmder);
                return 0;
                }

            dir = vfs_opendir (argv [1]);
            }
        else
            {
            char * path;

            if (__cwd == NULL)
                {
                cmder_printf (cmder, "error: file not exit!\n");
                return -1;
                }

            /* +2 for '/' and '\0' */

            path = malloc (strlen (__cwd) + strlen (argv [1]) + 2);

            if (path == NULL)
                {
                cmder_printf (cmder, "error: fail to build up path!\n");
                return -1;
                }

            sprintf (path, "%s/%s", __cwd, argv [1]);

            dir = vfs_opendir (path);

            free (path);
            }
        }

    if (dir == NULL)
        {
        cmder_printf (cmder, "error: fail to open dir\n");
        return -1;
        }

    while ((ent = vfs_readdir (dir)) != NULL)
        {
        cmder_printf (cmder, "%s\n", ent->de_name);
        };

    vfs_closedir (dir);

    return 0;
    }

CMDER_CMD_DEF ("ls", "list a directory", __cmd_ls);

static void __show_mount (cmder_t * cmder)
    {
    dlist_t            * itr;

    dlist_foreach (itr, &mp_head)
        {
        struct mount_point * mp;

        mp = container_of (itr, struct mount_point, mp_node);

        cmder_printf (cmder, "%s @ %s\n", mp->mp_fs->fs_name, mp->mp_path);
        }
    }

static int __cmd_mount (cmder_t * cmder, int argc, char * argv [])
    {
    if (argc == 1)
        {
        __show_mount (cmder);
        return 0;
        }

    return -1;
    }

// TODO: show information about mounted filesystems, or mount a filesystem

CMDER_CMD_DEF ("mount", "show information about mounted filesystems", __cmd_mount);

struct cmd_cat_info
    {
    cmder_t * cmder;
    int       fd;
    };

static int __cat_task (uintptr_t arg)
    {
    struct cmd_cat_info * info = (struct cmd_cat_info *) arg;
    char                  c;

    while (vfs_read (info->fd, &c, 1) != 0)
        {
        cmder_printf (info->cmder, "%02x", c);
        }

    return 0;
    }

static int __cmd_cat (cmder_t * cmder, int argc, char * argv [])
    {
    struct cmd_cat_info info;
    task_id             cat_task;

    if (argc == 1)
        {
        cmder_printf (cmder, "usage: ls <abs_path>\n");
        return -1;
        }

    info.cmder = cmder;

    if (argv [1][0] == '/')
        {
        info.fd = vfs_open (argv [1], O_RDONLY);
        }
    else
        {
        char * path;

        if (__cwd == NULL)
            {
            cmder_printf (cmder, "error: file not exit!\n");
            return -1;
            }

        /* +2 for '/' and '\0' */

        path = malloc (strlen (__cwd) + strlen (argv [1]) + 2);

        if (path == NULL)
            {
            cmder_printf (cmder, "error: fail to build up path!\n");
            return -1;
            }

        sprintf (path, "%s/%s", __cwd, argv [1]);

        info.fd = vfs_open (path, O_RDONLY);

        free (path);
        }

    if (info.fd == -1)
        {
        cmder_printf (cmder, "error: fail to open file\n");
        return -1;
        }

    cat_task = task_spawn ("cat", 30, 0, 0x500, __cat_task, (uintptr_t) &info);

    if (unlikely (cat_task == NULL))
        {
        cmder_printf (cmder, "error: fail to create cat task!\n");
        vfs_close (info.fd);
        return -1;
        }

    while (1)
        {
        unsigned char input = cmder->getc (cmder->arg);

        switch (input)
            {
            case 'q':
            case cmder_vk_ctrlc:
            case cmder_vk_ctrld:
                task_delete (cat_task);
                vfs_close (info.fd);
                return 0;
            default:
                break;
            }
        }
    }

CMDER_CMD_DEF ("cat", "concatenate a file to shell output", __cmd_cat);

static int __cd (cmder_t * cmder, char * path)
    {
    struct dir * dir;
    char       * cwd;

    if (strcmp (path, "/") == 0)
        {
        if (__cwd != NULL)
            {
            free (__cwd);
            __cwd = NULL;

            return 0;
            }
        }

    /* use opendir to test if dir valid */

    if ((dir = vfs_opendir (path)) == NULL)
        {
        cmder_printf (cmder, "error: fail to open dir!\n");
        return -1;
        }

    vfs_closedir (dir);

    if ((__cwd != NULL) && (strlen (__cwd) >= strlen (path)))
        {
        strcpy (__cwd, path);
        return 0;
        }

    cwd = __get_new_path (path);

    if (cwd == NULL)
        {
        cmder_printf (cmder, "error: fail to build up path!\n");
        return -1;
        }

    if (__cwd != NULL)
        {
        free (__cwd);
        }

    __cwd = cwd;

    return 0;
    }

static int __cmd_cd (cmder_t * cmder, int argc, char * argv [])
    {
    char * path;
    int    ret;
    char * cwd;

    if (argc == 1)
        {
        return __cd (cmder, "/");
        }

    if (argv [1][0] == '/')
        {
        return __cd (cmder, argv [1]);
        }

    cwd = __cwd == NULL ? "/" : __cwd;

    /* +2 for '/' and '\0' */

    path = malloc (strlen (__cwd) + strlen (argv [1]) + 2);

    if (path == NULL)
        {
        cmder_printf (cmder, "error: fail to build up path!\n");
        return -1;
        }

    sprintf (path, "%s/%s", cwd, argv [1]);

    ret = __cd (cmder, path);

    free (path);

    return ret;
    }

CMDER_CMD_DEF ("cd", "change the shell working directory", __cmd_cd);

static int __cmd_pwd (cmder_t * cmder, int argc, char * argv [])
    {
    cmder_printf (cmder, "%s\n", __cwd == NULL ? "/" : __cwd);

    return 0;
    }

CMDER_CMD_DEF ("pwd", "print the current working directory", __cmd_pwd);
#endif

