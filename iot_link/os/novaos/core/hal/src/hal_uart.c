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

#include <limits.h>
#include <string.h>

#include <common.h>
#include <hal_uart.h>
#include <irq.h>
#include <mutex.h>
#include <errno.h>
#include <bug.h>
#include <warn.h>
#include <init.h>

#ifdef CONFIG_DEVFS
#include <devfs.h>
#endif

/* locals */

static dlist_t __uarts = DLIST_INIT (__uarts);
static mutex_t __uarts_lock;

/**
 * hal_uart_open - open a uart by name
 * @name: the uart name
 *
 * return: a pointer to the uart control struct or NULL if device not found
 */

hal_uart_t * hal_uart_open (const char * name)
    {
    dlist_t    * itr;
    hal_uart_t * uart;

    WARN_ON (name == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_NAME; return NULL,
             "Invalid name!");

    mutex_lock (&__uarts_lock);

    dlist_foreach (itr, &__uarts)
        {
        uart = container_of (itr, hal_uart_t, node);

        if (strcmp (name, uart->name) == 0)
            {
            mutex_unlock (&__uarts_lock);
            return uart;
            }
        }

    mutex_unlock (&__uarts_lock);

    errno = ERRNO_HAL_UART_NO_MATCH;

    WARN ("Fail to open device \"%s\"!", name);

    return NULL;
    }

/**
 * hal_uart_poll_read - read from uart by polling mode
 * @uart: the uart device
 * @buff: the receiver buffer address
 * @len:  the receiver buffer length
 *
 * return: the number of bytes really read
 */

size_t hal_uart_poll_read (hal_uart_t * uart, unsigned char * buff, size_t len)
    {
    size_t i;

    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    WARN_ON (buff == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_BUFF; return 0,
             "Invalid buffer!");

    WARN_ON (uart->methods->poll_getc == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_OPERATION; return 0,
             "POLL mode get not supported!");

    for (i = 0; i < len; i++)
        {
        int ch = uart->methods->poll_getc (uart);

        if (ch == -1)
            {
            break;
            }

        buff [i] = (unsigned char) ch;
        }

    return i;
    }

static inline size_t __uart_getc (hal_uart_t * uart, unsigned char * buff,
                                  unsigned int timeout)
    {
    size_t ret;

    if (sem_timedwait (&uart->rxsem, timeout) != 0)
        {
        return 0;
        }

    if (mutex_timedlock (&uart->rxmux, timeout) != 0)
        {
        sem_post (&uart->rxsem);
        return 0;
        }

    ret = ring_getc (uart->rxring, buff);

    mutex_unlock (&uart->rxmux);

    return ret;
    }

/**
 * hal_uart_getc - get a char from uart
 * @uart: the uart device
 *
 * return: -1 on fail, the char if success
 */

int hal_uart_getc (hal_uart_t * uart)
    {
    unsigned char ch;
    size_t        got;

    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return -1,
             "Invalid uart device!");

    if (uart->mode == HAL_UART_MODE_POLL)
        {
        return uart->methods->poll_getc (uart);
        }
    else
        {
        got = __uart_getc (uart, &ch, WAIT_FOREVER);
        }

    return got == 0 ? -1 : (int) ch;
    }

/**
 * hal_uart_poll_getc - get a char from uart
 * @uart: the uart device
 *
 * return: the number of bytes really put
 */

int hal_uart_poll_getc (hal_uart_t * uart)
    {
    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    if (unlikely (uart->methods->poll_putc == NULL))
        {
        errno = ERRNO_HAL_UART_ILLEGAL_OPERATION;
        return 0;
        }

    return uart->methods->poll_getc (uart);
    }

/**
 * hal_uart_read - read from uart by asynchronous mode
 * @uart: the uart device
 * @buff: the receiver buffer address
 * @len:  the receiver buffer length
 *
 * return: the number of bytes really read
 */

size_t hal_uart_read (hal_uart_t * uart, unsigned char * buff, size_t len)
    {
    size_t i;

    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    WARN_ON (buff == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_BUFF; return 0,
             "Invalid buffer!");

    if (uart->mode == HAL_UART_MODE_POLL)
        {
        return hal_uart_poll_read (uart, buff, len);
        }

    for (i = 0; i < len; i++)
        {
        if (__uart_getc (uart, &buff [i], WAIT_FOREVER) == 0)
            {
            break;
            }
        }

    return len;
    }

// TODO: hal_uart_timedread

/**
 * hal_uart_poll_write - write buffer to uart by polling mode
 * @uart: the uart device
 * @buff: the writer buffer address
 * @len:  the writer buffer length
 *
 * return: the number of bytes really write
 */

size_t hal_uart_poll_write (hal_uart_t * uart, const unsigned char * buff, size_t len)
    {
    size_t i;

    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    WARN_ON (buff == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_BUFF; return 0,
             "Invalid buffer!");

    WARN_ON (uart->methods->poll_putc == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_OPERATION; return 0,
             "POLL mode put not supported!");

    for (i = 0; i < len; i++)
        {
        if (uart->methods->poll_putc (uart, buff [i]) != 0)
            {
            break;
            }
        }

    return i;
    }

static inline size_t __uart_putc (hal_uart_t * uart, unsigned char ch,
                                  unsigned int timeout)
    {
    size_t ret;
    bool   empty;

    if (sem_timedwait (&uart->txsem, timeout) != 0)
        {
        return 0;
        }

    if (mutex_timedlock (&uart->txmux, timeout) != 0)
        {
        sem_post (&uart->txsem);
        return 0;
        }

    empty = ring_empty (uart->txring);

    ret = ring_putc (uart->txring, ch);

    mutex_unlock (&uart->txmux);

    if (empty)
        {
        uart->methods->tx_start (uart);
        }

    return ret;
    }

/**
 * hal_uart_putc - write a char to uart
 * @uart: the uart device
 * @ch:   the char to write
 *
 * return: the number of bytes really put
 */

size_t hal_uart_putc (hal_uart_t * uart, const unsigned char ch)
    {
    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    if (uart->mode == HAL_UART_MODE_POLL)
        {
        return uart->methods->poll_putc (uart, ch);
        }

    return __uart_putc (uart, ch, WAIT_FOREVER);
    }

/**
 * hal_uart_poll_putc - write a char to uart
 * @uart: the uart device
 * @ch:   the char to write
 *
 * return: the number of bytes really put
 */

size_t hal_uart_poll_putc (hal_uart_t * uart, const unsigned char ch)
    {
    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    WARN_ON (uart->methods->poll_putc == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_OPERATION; return 0,
             "POLL mode put not supported!");

    return uart->methods->poll_putc (uart, ch);
    }

/**
 * hal_uart_write - write buffer to uart
 * @uart: the uart device
 * @buff: the writer buffer address
 * @len:  the writer buffer length
 *
 * return: the number of bytes really write
 */

size_t hal_uart_write (hal_uart_t * uart, const unsigned char * buff, size_t len)
    {
    size_t i;

    WARN_ON (uart == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_ID; return 0,
             "Invalid uart device!");

    WARN_ON (buff == NULL,
             errno = ERRNO_HAL_UART_ILLEGAL_BUFF; return 0,
             "Invalid buffer!");

    if (uart->mode == HAL_UART_MODE_POLL)
        {
        return hal_uart_poll_write (uart, buff, len);
        }

    for (i = 0; i < len; i++)
        {
        if (__uart_putc (uart, buff [i], WAIT_FOREVER) == 0)
            {
            return i;
            }
        }

    return len;
    }

// TODO: hal_uart_timedwrite

/**
 * hal_rx_putc - put char by uart rx-ISR or a deferred rx-ISR task
 * @uart: the uart device
 * @ch:   the char to put to the rxring
 *
 * return: NA
 */

void hal_rx_putc (hal_uart_t * uart, unsigned char ch)
    {
    struct ring * ring;
    bool          full;

    BUG_ON (uart == NULL, "Invalid uart device!");

    /* uart ISR must be handled in task context */

    BUG_ON (int_context (), "RX should always in task context!");

    ring = uart->rxring;

    mutex_lock (&uart->rxmux);

    full = ring_full (ring);

    ring_putc_force (ring, ch);

    mutex_unlock (&uart->rxmux);

    if (!full)
        {
        sem_post (&uart->rxsem);
        }
    }

/**
 * hal_tx_getc - get char by uart tx-ISR or a deferred tx-ISR task
 * @uart: the uart device
 * @ch:   a pointer to char where put the char get from txring
 *
 * return: 1 if got a char from txring, 0 if the txring is empty
 */

size_t hal_tx_getc (hal_uart_t * uart, unsigned char * ch)
    {
    size_t        ret;
    struct ring * ring;

    BUG_ON (uart == NULL, "Invalid uart device!");

    /* uart ISR must be handled in task context */

    BUG_ON (int_context (), "TX should always in task context!");

    ring = uart->txring;

    mutex_lock (&uart->txmux);

    ret = ring_empty (ring) ? 0 : ring_getc (ring, ch);

    mutex_unlock (&uart->txmux);

    if (ret != 0)
        {
        sem_post (&uart->txsem);
        }

    return ret;
    }

#ifdef CONFIG_DEVFS
static int __uart_read (uintptr_t data, char * buff, size_t nbyte)
    {
    int ret = (int) hal_uart_read ((hal_uart_t *) data, (unsigned char *) buff, nbyte);

    return ret == 0 ? -1 : ret;
    }

static int __uart_write (uintptr_t data, const char * buff, size_t nbyte)
    {
    int ret = (int) hal_uart_write ((hal_uart_t *) data, (const unsigned char *) buff, nbyte);

    return ret == 0 ? -1 : ret;
    }

static int __uart_ioctl (uintptr_t data, int request, va_list valist)
    {
    return -1;
    }

static const struct devfs_ops __uart_ops =
    {
    NULL,
    NULL,
    __uart_read,
    __uart_write,
    NULL,
    __uart_ioctl,
    NULL,
    NULL,
    NULL
    };
#endif

/**
 * hal_uart_register - register a uart hal device
 * @uart:    the uart device to register
 * @name:    the uart device name
 * @methods: the uart operation methods
 * @mode:    the uart operating mode
 *
 * return: 0 on success, negtive value on error
 */

int hal_uart_register (hal_uart_t * uart, const char * name,
                       const struct hal_uart_methods * methods, uint8_t mode)
    {
    BUG_ON (uart    == NULL, "Invalid uart device!");
    BUG_ON (name    == NULL, "Invalid uart device name!");
    BUG_ON (methods == NULL, "Invalid uart device methods!");

    switch (mode)
        {
        case HAL_UART_MODE_INT:
            BUG_ON (uart->methods->tx_start == NULL, "Invalid uart device methods");

            break;
        case HAL_UART_MODE_POLL:
            BUG_ON ((uart->methods->poll_putc == NULL) || (uart->methods->poll_getc == NULL),
                    "Invalid uart device methods");

            break;
        default:
            BUG ("Invalid uart device methods");
            break;
        }

    uart->name    = name;
    uart->mode    = mode;
    uart->methods = methods;

    if (unlikely (sem_init (&uart->rxsem, 0) != 0))
        {
        goto err_out;
        }

    if (unlikely (sem_init (&uart->txsem, HAL_UART_RING_SIZE) != 0))
        {
        goto err_out_destroy_rxsem;
        }

    if (unlikely (mutex_init (&uart->rxmux) != 0))
        {
        goto err_out_destroy_txsem;
        }

    if (unlikely (mutex_init (&uart->txmux) != 0))
        {
        goto err_out_destroy_rxmux;
        }

    uart->rxring = ring_create (HAL_UART_RING_SIZE);

    if (unlikely (uart->rxring == NULL))
        {
        goto err_out_destroy_txmux;
        }

    uart->txring = ring_create (HAL_UART_RING_SIZE);

    if (unlikely (uart->txring == NULL))
        {
        goto err_out_destroy_rxring;
        }

    if (unlikely (mutex_lock (&__uarts_lock) != 0))
        {
        goto err_out_destroy_txring;
        }

    dlist_add_tail (&__uarts, &uart->node);

    mutex_unlock (&__uarts_lock);

#ifdef CONFIG_DEVFS
    return devfs_add_file (name, &__uart_ops, (uintptr_t) uart);
#else
    return 0;
#endif

err_out_destroy_txring:
    ring_destroy (uart->txring);
err_out_destroy_rxring:
    ring_destroy (uart->rxring);
err_out_destroy_txmux:
    mutex_destroy (&uart->txmux);
err_out_destroy_rxmux:
    mutex_destroy (&uart->rxmux);
err_out_destroy_txsem:
    sem_destroy (&uart->txsem);
err_out_destroy_rxsem:
    sem_destroy (&uart->rxsem);
err_out:
    return -1;
    }

static int hal_uart_init (void)
    {
    return mutex_init (&__uarts_lock);
    }

MODULE_INIT (hal, hal_uart_init);

