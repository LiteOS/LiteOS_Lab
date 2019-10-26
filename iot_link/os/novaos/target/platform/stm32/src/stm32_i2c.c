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

#include <kconfig.h>

#include CONFIG_STM32_HAL_H

#include <config.h>
#include <hal_i2c.h>
#include <init.h>
#include <warn.h>
#include <symn2c.h>

/* typedefs */

struct stm32_i2c
    {
    hal_i2c_t         hal_i2c;
    I2C_HandleTypeDef handle;
    /* more members if needed */
    };

/* locals */

#ifdef CONFIG_STM32_I2C1
static struct stm32_i2c stm32_i2c1;
#endif

#ifdef CONFIG_STM32_I2C2
static struct stm32_i2c stm32_i2c2;
#endif

#ifdef CONFIG_STM32_I2C3
static struct stm32_i2c stm32_i2c3;
#endif

static int __stm32_i2c_xfer (hal_i2c_t * hal_i2c, uint16_t addr,
                             const uint8_t * data, uint16_t size)
    {
    HAL_StatusTypeDef status;

    struct stm32_i2c * i2c = container_of (hal_i2c, struct stm32_i2c, hal_i2c);

    status = HAL_I2C_Master_Transmit (&i2c->handle, addr, (uint8_t *) data, size,
                                      0xffffffff);

    return status == HAL_OK ? (int) size : 0;
    }

static int __stm32_i2c_recv (hal_i2c_t * hal_i2c, uint16_t addr,
                             uint8_t * data, uint16_t size)
    {
    HAL_StatusTypeDef status;

    struct stm32_i2c * i2c = container_of (hal_i2c, struct stm32_i2c, hal_i2c);

    status = HAL_I2C_Master_Receive  (&i2c->handle, addr, data, size, 0xffffffff);

    return status == HAL_OK ? (int) size : 0;
    }

static const hal_i2c_methods_t stm32_i2c_methods =
    {
    NULL,
    __stm32_i2c_xfer,
    __stm32_i2c_recv,
    };

static int __stm32_i2c_init (struct stm32_i2c * i2c, I2C_TypeDef * base,
                             GPIO_TypeDef * sclgpiox, uint32_t sclpin,
                             GPIO_TypeDef * sdagpiox, uint32_t sdapin,
                             uint32_t alter, const char * name)
    {
    GPIO_InitTypeDef io;

    i2c->handle.Instance              = base;
    i2c->handle.Init.Timing           = 0x10909CEC;
    i2c->handle.Init.OwnAddress1      = 0;
    i2c->handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    i2c->handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    i2c->handle.Init.OwnAddress2      = 0;
    i2c->handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    i2c->handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    i2c->handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init (&i2c->handle) != HAL_OK)
        {
        WARN ("HAL_I2C_Init fail!");
        return -1;
        }

    io.Pin       = 1 << sclpin;
    io.Mode      = GPIO_MODE_AF_OD;
    io.Pull      = GPIO_PULLUP;
    io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    io.Alternate = alter;

    HAL_GPIO_Init (sclgpiox, &io);

    io.Pin       = 1 << sdapin;
    io.Mode      = GPIO_MODE_AF_OD;
    io.Pull      = GPIO_PULLUP;
    io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    io.Alternate = alter;

    HAL_GPIO_Init (sdagpiox, &io);

    return hal_i2c_register (&i2c->hal_i2c, name, &stm32_i2c_methods);
    }

static int stm32_i2c_init (void)
    {
#ifdef CONFIG_STM32_I2C1
    __HAL_RCC_I2C1_CLK_ENABLE ();

    if (__stm32_i2c_init (&stm32_i2c1, I2C1,
                          SYM_N2C (GPIO, CONFIG_STM32_I2C1_SCL_GPIOX), CONFIG_STM32_I2C1_SCL_PIN,
                          SYM_N2C (GPIO, CONFIG_STM32_I2C1_SDA_GPIOX), CONFIG_STM32_I2C1_SDA_PIN,
                          CONFIG_STM32_I2C1_AF, "i2c1") != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_I2C2
    __HAL_RCC_I2C2_CLK_ENABLE ();

    if (__stm32_i2c_init (&stm32_i2c2, I2C2,
                          SYM_N2C (GPIO, CONFIG_STM32_I2C2_SCL_GPIOX), CONFIG_STM32_I2C2_SCL_PIN,
                          SYM_N2C (GPIO, CONFIG_STM32_I2C2_SDA_GPIOX), CONFIG_STM32_I2C2_SDA_PIN,
                          CONFIG_STM32_I2C2_AF, "i2c2") != 0)
        {
        return -1;
        }
#endif

#ifdef CONFIG_STM32_I2C3
    __HAL_RCC_I2C3_CLK_ENABLE ();

    if (__stm32_i2c_init (&stm32_i2c3, I2C3,
                          SYM_N2C (GPIO, CONFIG_STM32_I2C3_SCL_GPIOX), CONFIG_STM32_I2C3_SCL_PIN,
                          SYM_N2C (GPIO, CONFIG_STM32_I2C3_SDA_GPIOX), CONFIG_STM32_I2C3_SDA_PIN,
                          CONFIG_STM32_I2C3_AF, "i2c3") != 0)
        {
        return -1;
        }
#endif

    return 0;
    }

MODULE_INIT (bus, stm32_i2c_init);

