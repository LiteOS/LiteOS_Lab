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

#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <list.h>
#include <mutex.h>
#include <module.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define ERRNO_HAL_I2C_ILLEGAL_ID            ERRNO_JOIN (MID_HAL, 0x31)
#define ERRNO_HAL_I2C_ILLEGAL_BUFF          ERRNO_JOIN (MID_HAL, 0x32)
#define ERRNO_HAL_I2C_ILLEGAL_CONFIG        ERRNO_JOIN (MID_HAL, 0x33)
#define ERRNO_HAL_I2C_ILLEGAL_OPERATION     ERRNO_JOIN (MID_HAL, 0x34)
#define ERRNO_HAL_I2C_ILLEGAL_NAME          ERRNO_JOIN (MID_HAL, 0x35)
#define ERRNO_HAL_I2C_NO_MATCH              ERRNO_JOIN (MID_HAL, 0x36)

/* typedefs */

typedef struct hal_i2c hal_i2c_t;

typedef struct hal_i2c_methods
    {
    int       (* ioctl) (hal_i2c_t *, int, va_list);
    int       (* xfer)  (hal_i2c_t *, uint16_t, const uint8_t *, uint16_t);
    int       (* recv)  (hal_i2c_t *, uint16_t, uint8_t *, uint16_t);
    } hal_i2c_methods_t;

struct hal_i2c
    {
    dlist_t      node;              /* node to be inserted in the timer list */
    const char * name;

    mutex_t      lock;

    const hal_i2c_methods_t * methods;
    };

typedef struct hal_i2c_dev
    {
    hal_i2c_t  * i2c;
    const char * name;
    uint16_t     addr;
    dlist_t      node;
    int       (* init)   (struct hal_i2c_dev *);
    int       (* deinit) (struct hal_i2c_dev *);
    } hal_i2c_dev_t;

/* externs */

extern hal_i2c_t     * hal_i2c_open         (const char *);
extern int             hal_i2c_xfer         (hal_i2c_t *, uint16_t, const uint8_t *,
                                             uint16_t);
extern int             hal_i2c_recv         (hal_i2c_t *, uint16_t, uint8_t *, uint16_t);
extern int             hal_i2c_register     (hal_i2c_t *, const char *,
                                             const struct hal_i2c_methods *);
extern int             hal_i2c_dev_register (hal_i2c_dev_t *, const char *,
                                             const char *, uint16_t,
                                             int (*) (struct hal_i2c_dev *),
                                             int (*) (struct hal_i2c_dev *));
extern hal_i2c_dev_t * hal_i2c_dev_open     (const char *);
extern int             hal_i2c_dev_xfer     (hal_i2c_dev_t *, const uint8_t *, uint16_t);
extern int             hal_i2c_dev_recv     (hal_i2c_dev_t *, uint8_t *, uint16_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HAL_I2C_H__ */

