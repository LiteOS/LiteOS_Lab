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

#include <config.h>
#include <phys_mem.h>

#ifdef __KEIL__
#define __stack_end__                       Image$$ARM_LIB_STACKHEAP$$ZI$$Limit
#endif

extern char __stack_end__ [];

const struct phys_mem system_phys_mem [] =
    {
        { __stack_end__, (char *) (CONFIG_RAM_START + CONFIG_RAM_SIZE), },
#ifdef  CONFIG_EXTRA_RAM0
        { (char *) CONFIG_EXTRA_RAM0_START, (char *) (CONFIG_EXTRA_RAM0_START + CONFIG_EXTRA_RAM0_SIZE) },
#endif
#ifdef  CONFIG_EXTRA_RAM1
        { (char *) CONFIG_EXTRA_RAM1_START, (char *) (CONFIG_EXTRA_RAM1_START + CONFIG_EXTRA_RAM1_SIZE) },
#endif
#ifdef  CONFIG_EXTRA_RAM2
        { (char *) CONFIG_EXTRA_RAM2_START, (char *) (CONFIG_EXTRA_RAM2_START + CONFIG_EXTRA_RAM2_SIZE) },
#endif
        { 0, 0 }
    };
