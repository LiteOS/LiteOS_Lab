/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *      http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <stdbool.h>

#include <config.h>
#include <kprintf.h>
#include <cmder.h>

#ifdef CONFIG_DEVFS
#include <vfs.h>
#else
#include <hal_i2c.h>
#endif

volatile bool __test_exit;

static void __dump_lux (uint8_t * buff)
    {
    uint32_t lux = buff [0];

    lux = (lux << 8) + buff [1];

    /* return lux / 1.2, lux / 1.2 = lux / (6 / 5) = lux * 5 / 6 */

    kprintf ("lux = %d\n", lux * 5 / 6);
    }

static void __bh1750_test (void)
    {
#ifdef CONFIG_DEVFS
    uint8_t buff [2];
    int     fd = vfs_open ("/dev/bh1750", O_RDWR);

    if (fd < 0)
        {
        kprintf ("fail to open /dev/bh1750!\n");
        return;
        }

    while (!__test_exit)
        {
        task_delay (CONFIG_SYS_TICK_HZ);

        vfs_read (fd, (char *) &buff, 2);

        __dump_lux (buff);
        }
#else
    uint8_t         buff [2];
    hal_i2c_dev_t * bh1750_dev = hal_i2c_dev_open ("bh1750");

    if (bh1750_dev == NULL)
        {
        kprintf ("fail to open device bh1750!\n");
        return;
        }

    while (!__test_exit)
        {
        task_delay (CONFIG_SYS_TICK_HZ);

        hal_i2c_dev_recv (bh1750_dev, (uint8_t *) &buff, 2);

        __dump_lux (buff);
        }
#endif
    }

static int __cmd_bh1750_test (cmder_t * cmder, int argc, char * argv [])
    {
    static task_id __bh1750_tid = NULL;

    if (__bh1750_tid == NULL)
        {
        __test_exit  = false;
        __bh1750_tid = task_spawn ("bh1750_test", 20, 0, 0x800, (int (*) (uintptr_t)) __bh1750_test, 0);
        }
    else
        {
        __test_exit  = true;
        __bh1750_tid = NULL;
        }

    return 0;
    }

CMDER_CMD_DEF ("bh1750_test", "bh1750 test", __cmd_bh1750_test);
