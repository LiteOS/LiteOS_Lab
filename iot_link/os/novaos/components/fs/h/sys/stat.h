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

#ifndef __STAT_H__
#define __STAT_H__

#include <stdint.h>
#include <stdbool.h>

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedefs */

struct stat
    {
    uint32_t st_dev;
    uint32_t st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint16_t st_uid;
    uint16_t st_gid;
    uint32_t st_rdev;
    uint32_t st_size;
    uint32_t st_atime;
    uint32_t st_mtime;
    uint32_t st_ctime;
    uint32_t st_blksize;
    uint32_t st_blocks;
    };

/* macros */

#define S_IXOTH         (0000001)       /* execute/search permission, other */
#define S_IWOTH         (0000002)       /* write permission, other */
#define S_IROTH         (0000004)       /* read permission, other */
#define S_IRWXO         (S_IROTH | S_IWOTH | S_IXOTH)

#define S_IXGRP         (0000010)       /* execute/search permission, group */
#define S_IWGRP         (0000020)       /* write permission, grougroup */
#define S_IRGRP         (0000040)       /* read permission, group */
#define S_IRWXG         (S_IRGRP | S_IWGRP | S_IXGRP)

#define S_IXUSR         (0000100)       /* execute/search permission, owner */
#define S_IWUSR         (0000200)       /* write permission, owner */
#define S_IRUSR         (0000400)       /* read permission, owner */
#define S_IRWXU         (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_ISVTX         (0001000)       /* save swapped text even after use */
#define S_ISGID         (0002000)       /* set group id on execution */
#define S_ISUID         (0004000)       /* set user id on execution */

#define S_IFIFO         (0010000)       /* fifo */
#define S_IFCHR         (0020000)       /* character special */
#define S_IFDIR         (0040000)       /* directory */
#define S_IFBLK         (0060000)       /* block special */
#define S_IFREG         (0100000)       /* regular */
#define S_IFLNK         (0120000)       /* symbolic link */
#define S_IFSOCK        (0140000)       /* socket */
#define S_IFMT          (0170000)       /* type of file */

#define S_ISBLK         __stat_is_blk
#define S_ISCHR         __stat_is_chr
#define S_ISDIR         __stat_is_dir
#define S_ISFIFO        __stat_is_fifo
#define S_ISREG         __stat_is_reg
#define S_ISLNK         __stat_is_lnk
#define S_ISSOCK        __stat_is_sock

/* inlines */

static __always_inline bool __stat_is_blk (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFBLK;
    }

static __always_inline bool __stat_is_chr (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFCHR;
    }

static __always_inline bool __stat_is_dir (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFDIR;
    }

static __always_inline bool __stat_is_fifo (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFIFO;
    }

static __always_inline bool __stat_is_reg (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFREG;
    }

static __always_inline bool __stat_is_lnk (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFLNK;
    }

static __always_inline bool __stat_is_sock (uint32_t st_mode)
    {
    return (st_mode & S_IFMT) == S_IFSOCK;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STAT_H__ */
