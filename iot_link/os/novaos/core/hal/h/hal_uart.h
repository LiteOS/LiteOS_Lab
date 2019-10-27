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

#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <list.h>
#include <ring.h>
#include <sem.h>
#include <mutex.h>
#include <module.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define HAL_UART_RING_SIZE                  256

#define HAL_UART_MODE_POLL                  (1 << 0)
#define HAL_UART_MODE_INT                   (1 << 1)

#define ERRNO_HAL_UART_ILLEGAL_ID           ERRNO_JOIN (MID_HAL, 0x21)
#define ERRNO_HAL_UART_ILLEGAL_BUFF         ERRNO_JOIN (MID_HAL, 0x22)
#define ERRNO_HAL_UART_ILLEGAL_OPERATION    ERRNO_JOIN (MID_HAL, 0x23)
#define ERRNO_HAL_UART_ILLEGAL_NAME         ERRNO_JOIN (MID_HAL, 0x24)
#define ERRNO_HAL_UART_NO_MATCH             ERRNO_JOIN (MID_HAL, 0x25)

/* typedefs */

typedef struct hal_uart hal_uart_t;

typedef struct hal_uart_methods
    {
    int          (* ioctl)     (hal_uart_t *, int, va_list);
    int          (* poll_getc) (hal_uart_t *);
    size_t       (* poll_putc) (hal_uart_t *, unsigned char);
    int          (* tx_start)  (hal_uart_t *);
    } hal_uart_methods_t;

struct hal_uart
    {
    dlist_t      node;              /* node to be inserted in the timer list */
    const char * name;
    uint8_t      mode;

    ring_t     * rxring;
    ring_t     * txring;

    sem_t        rxsem;
    sem_t        txsem;
    mutex_t      rxmux;
    mutex_t      txmux;

    const hal_uart_methods_t * methods;
    };

/* externs */

extern hal_uart_t * hal_uart_open       (const char *);
extern size_t       hal_uart_poll_read  (hal_uart_t *, unsigned char *, size_t);
extern int          hal_uart_getc       (hal_uart_t *);
extern int          hal_uart_poll_getc  (hal_uart_t *);
extern size_t       hal_uart_read       (hal_uart_t *, unsigned char *, size_t);
extern size_t       hal_uart_poll_write (hal_uart_t *, const unsigned char *, size_t);
extern size_t       hal_uart_putc       (hal_uart_t *, const unsigned char);
extern size_t       hal_uart_poll_putc  (hal_uart_t *, const unsigned char);
extern size_t       hal_uart_write      (hal_uart_t *, const unsigned char *, size_t);
extern void         hal_rx_putc         (hal_uart_t *, unsigned char);
extern size_t       hal_tx_getc         (hal_uart_t *, unsigned char *);
extern int          hal_uart_register   (hal_uart_t *, const char *,
                                         const struct hal_uart_methods *, uint8_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HAL_UART_H__ */

