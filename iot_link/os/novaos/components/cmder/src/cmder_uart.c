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

#include <stdio.h>

#include <config.h>
#include <hal_uart.h>
#include <cmder.h>
#include <init.h>
#include <task.h>
#include <warn.h>

/* locals */

static task_id cmder = NULL;

static unsigned char uart_getchar (uintptr_t arg)
    {
    unsigned char ch;

    hal_uart_t * uart = (hal_uart_t *) arg;

    while ((ch = (unsigned char) hal_uart_getc (uart)) == '\0');

    if (ch != 0x1b)
        {
        return ch;
        }

    ch = (unsigned char) hal_uart_getc (uart);

    if (ch != 0x5b)
        {
        return ch;
        }

    ch = (unsigned char) hal_uart_getc (uart);

    switch (ch)
        {
        case 0x31:
            ch = (unsigned char) hal_uart_getc (uart);  /* eat 0x7e */
            return cmder_vk_home;
        case 0x33:
            ch = (unsigned char) hal_uart_getc (uart);  /* eat 0x7e */
            return cmder_vk_del;
        case 0x34:
            ch = (unsigned char) hal_uart_getc (uart);  /* eat 0x7e */
            return cmder_vk_end;
        case 0x41:
            return cmder_vk_up;
        case 0x42:
            return cmder_vk_down;
        case 0x43:
            return cmder_vk_right;
        case 0x44:
            return cmder_vk_left;
        default:
            return '\0';
        }
    }

static int uart_putchar (uintptr_t arg, unsigned char ch)
    {
    hal_uart_t * uart = (hal_uart_t *) arg;

    hal_uart_putc (uart, ch);

    return 1;
    }

static cmder_t uart_cmder =
    {
    "", 0, 0, 0, 0, 0, {0}, {0}, uart_getchar, uart_putchar, 0
    };

/**
 * uart_cmder_init - uart cmder shell initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int uart_cmder_init (void)
    {
    hal_uart_t * uart;

    static unsigned char his_content [CONFIG_CMDER_UART_HIS_CMD_BUFF_SIZE];
    static unsigned char his_indexes [CONFIG_CMDER_UART_HIS_IDX_BUFF_SIZE];

    if (cmder != NULL)
        {
        return 0;
        }

    ring_init (&uart_cmder.his_cmd, his_content, CONFIG_CMDER_UART_HIS_CMD_BUFF_SIZE);
    ring_init (&uart_cmder.his_idx, his_indexes, CONFIG_CMDER_UART_HIS_IDX_BUFF_SIZE);

    uart = hal_uart_open (CONFIG_CMDER_UART_NAME);

    if (uart == NULL)
        {
        return -1;
        }

    uart_cmder.arg = (uintptr_t) uart;

    cmder = task_spawn ("cmder", 1, TASK_OPTION_SYSTEM, CONFIG_CMDER_UART_STACK_SIZE,
                        (int (*) (uintptr_t)) cmder_loop, (uintptr_t) &uart_cmder);

    WARN_ON (cmder == NULL, return -1, "Fail to create cmder task!");

    return 0;
    }

MODULE_INIT (postdriver, uart_cmder_init);

