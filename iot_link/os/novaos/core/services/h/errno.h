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

#ifndef __ERRNO_H__
#define __ERRNO_H__

#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define errno                   (*errno_get ())

/* posix errno */

#define EPERM                   1       /* not owner */
#define ENOENT                  2       /* no such file or directory */
#define ESRCH                   3       /* no such context */
#define EINTR                   4       /* interrupted system call */
#define EIO                     5       /* i/O error */
#define ENXIO                   6       /* no such device or address */
#define E2BIG                   7       /* arg list too long */
#define ENOEXEC                 8       /* exec format error */
#define EBADF                   9       /* bad file number */
#define ECHILD                  10      /* no children */
#define EAGAIN                  11      /* no more contexts */
#define ENOMEM                  12      /* not enough core */
#define EACCES                  13      /* permission denied */
#define EFAULT                  14      /* bad address */
#define ENOTEMPTY               15      /* directory not empty */
#define EBUSY                   16      /* mount device busy */
#define EEXIST                  17      /* file exists */
#define EXDEV                   18      /* cross-device link */
#define ENODEV                  19      /* no such device */
#define ENOTDIR                 20      /* not a directory */
#define EISDIR                  21      /* is a directory */
#define EINVAL                  22      /* invalid argument */
#define ENFILE                  23      /* file table overflow */
#define EMFILE                  24      /* too many open files */
#define ENOTTY                  25      /* not a typewriter */
#define ENAMETOOLONG            26      /* file name too long */
#define EFBIG                   27      /* file too large */
#define ENOSPC                  28      /* no space left on device */
#define ESPIPE                  29      /* illegal seek */
#define EROFS                   30      /* read-only file system */
#define EMLINK                  31      /* too many links */
#define EPIPE                   32      /* broken pipe */
#define EDEADLK                 33      /* resource deadlock avoided */
#define ENOLCK                  34      /* no locks available */
#define ENOTSUP                 35      /* unsupported value */
#define EMSGSIZE                36      /* message size */

/* ANSI math software */

#define EDOM                    37      /* argument too large */
#define ERANGE                  38      /* result too large */

/* ipc/network software */

/* argument errors */

#define EDESTADDRREQ            40      /* destination address required */
#define EPROTOTYPE              41      /* protocol wrong type for socket */
#define ENOPROTOOPT             42      /* protocol not available */
#define EPROTONOSUPPORT         43      /* protocol not supported */
#define ESOCKTNOSUPPORT         44      /* socket type not supported */
#define EOPNOTSUPP              45      /* operation not supported on socket */
#define EPFNOSUPPORT            46      /* protocol family not supported */
#define EAFNOSUPPORT            47      /* addr family not supported */
#define EADDRINUSE              48      /* address already in use */
#define EADDRNOTAVAIL           49      /* can't assign requested address */
#define ENOTSOCK                50      /* socket operation on non-socket */

/* operational errors */

#define ENETUNREACH             51      /* network is unreachable */
#define ENETRESET               52      /* network dropped connection on reset */
#define ECONNABORTED            53      /* software caused connection abort */
#define ECONNRESET              54      /* connection reset by peer */
#define ENOBUFS                 55      /* no buffer space available */
#define EISCONN                 56      /* socket is already connected */
#define ENOTCONN                57      /* socket is not connected */
#define ESHUTDOWN               58      /* can't send after socket shutdown */
#define ETOOMANYREFS            59      /* too many references: can't splice */
#define ETIMEDOUT               60      /* connection timed out */
#define ECONNREFUSED            61      /* connection refused */
#define ENETDOWN                62      /* network is down */
#define ETXTBSY                 63      /* text file busy */
#define ELOOP                   64      /* too many levels of symbolic links */
#define EHOSTUNREACH            65      /* no route to host */
#define ENOTBLK                 66      /* block device required */
#define EHOSTDOWN               67      /* host is down */

/* non-blocking and interrupt i/o */

#define EINPROGRESS             68  /* operation now in progress */
#define EALREADY                69      /* operation already in progress */
#define EWOULDBLOCK             EAGAIN  /* operation would block */

#define ENOSYS                  71      /* function not implemented */

/* aio errors (should be under posix) */

#define ECANCELED               72      /* operation canceled */

#define ERRMAX                  81

/* specific STREAMS errno values */

#define ENOSR                   74      /* insufficient memory */
#define ENOSTR                  75      /* streams device required */
#define EPROTO                  76      /* generic STREAMS error */
#define EBADMSG                 77      /* invalid STREAMS message */
#define ENODATA                 78      /* missing expected message data */
#define ETIME                   79      /* sTREAMS timeout occurred */
#define ENOMSG                  80      /* unexpected message type */

#define EILSEQ                  138     /* illegal byte sequence */

/*
 * kernel specific errno
 *
 * the errno format:
 *
 * +-----+-----+
 * | mid | err |
 * +-----+-----+
 *    |     |
 *    |     \
 *    \      `-- 8-bit error number
 *     `-------- 8-bit module id
 */

#define ERRNO_JOIN(mid, err)    ((((uint32_t) mid & 0xff) << 8) | \
                                 (((uint32_t) err & 0xff) << 0))

/* externs */

extern uint32_t * errno_get     (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ERRNO_H__ */

