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

#include <hal_i2c.h>
#include <init.h>

#define BH1750_ADDR          0x46

static struct hal_i2c_dev bh1750;

static int __init (struct hal_i2c_dev * dev)
    {
    const uint8_t cmd_init  = 0x01;
    const uint8_t cmd_start = 0x10;

    hal_i2c_dev_xfer (dev, (const uint8_t *) &cmd_init,  1);

    task_delay (1);

    hal_i2c_dev_xfer (dev, (const uint8_t *) &cmd_start, 1);

    return 0;
    }

static int bh1750_init (void)
    {
    return hal_i2c_dev_register (&bh1750, "i2c1", "bh1750", BH1750_ADDR, __init,
                                 NULL);
    }

MODULE_INIT (driver, bh1750_init);
