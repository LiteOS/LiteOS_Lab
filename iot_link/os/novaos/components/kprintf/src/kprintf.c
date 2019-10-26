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

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#include <xprintf.h>
#include <hal_uart.h>
#include <critical.h>
#include <init.h>
#include <warn.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

static struct hal_uart * __kprintf_uart = NULL;

/*
 * platform or bsp should define there own early print routine to override this
 * kprintf output strings like this: kprintf -> xprintf -> __uart_putc
 * __uart_putc depends on the hal_uart, but hal_uart is only usable after system
 * up, so kprintf can not be used before uart driver initialization, so add a
 * weak symbol early_putc to output before hal_uart usable, soc or bsp can
 * implement such a routine (usually in poll mode)
 */

__weak int early_putc (unsigned char ch)
    {
    return 0;
    }

static int __uart_putc (unsigned char ch)
    {
    if (__kprintf_uart == NULL)
        {
        return early_putc (ch);
        }

    return sleepable () ? (int) hal_uart_putc (__kprintf_uart, ch)
                        : (int) hal_uart_poll_putc (__kprintf_uart, ch);
    }

static int __kprintf_putc (uintptr_t arg, unsigned char ch)
    {
    __uart_putc (ch);

    if (ch == '\n')
        {
        __uart_putc ('\r');
        }

    return 1;
    }

int kprintf (const char * format, ...)
    {
    va_list valist;
    int     nbytes;

    va_start (valist, format);
    nbytes = xprintf (format, valist, __kprintf_putc, 0);
    va_end (valist);

    return nbytes;
    }


int printf (const char * format, ...)
    {
    va_list valist;
    int     nbytes;

    va_start (valist, format);
    nbytes = xprintf (format, valist, __kprintf_putc, 0);
    va_end (valist);

    return nbytes;
    }

int puts (const char *str)
{

    kprintf(str);

    return 0;
}


/**
 * kprintf_lib_init - kprintf library initialization routine
 *
 * return : 0 on success, negtive value on error
 */

static int kprintf_lib_init (void)
    {
    __kprintf_uart = hal_uart_open (CONFIG_KPRINTF_UART_NAME);

    WARN_ON (__kprintf_uart == NULL, return -1, "Fail to open uart for kprintf!");

    return 0;
    }

MODULE_INIT (postdriver, kprintf_lib_init);

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_kprintf [] =
    {
    (uintptr_t) kprintf,
    };

/* must be global to override weak symbol */

const struct syscall_table syscall_table_kprintf =
    {
    ARRAY_SIZE (syscall_entries_kprintf),
    syscall_entries_kprintf
    };
#endif

