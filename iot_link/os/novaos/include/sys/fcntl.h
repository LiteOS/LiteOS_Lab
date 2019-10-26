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

#ifndef __FCNTL_H__
#define __FCNTL_H__

#define O_RDONLY                (0x0000)
#define O_WRONLY                (0x0001)
#define O_RDWR                  (0x0002)
#define O_APPEND                (0x0008)
#define O_CREAT                 (0x0200)
#define O_TRUNC                 (0x0400)
#define O_EXCL                  (0x0800)
#define O_SYNC                  (0x2000)
#define O_NONBLOCK              (0x4000)
#define O_NOCTTY                (0x8000)

#define O_ACCMODE               (O_RDONLY | O_WRONLY | O_RDWR)

#endif /* __FCNTL_H__ */
