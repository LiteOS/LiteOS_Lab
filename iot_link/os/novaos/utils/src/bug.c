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

#include <irq.h>

#ifdef CONFIG_KPRINTF
#include <kprintf.h>
#endif

#include <arch/trace.h>

volatile int __bug_dummy_check = 0;

void __bug (const char * info)
    {
    (void) int_lock ();

#ifdef CONFIG_KPRINTF
    /* if __bug beening invoked directly */

    if (info [0] != '\0')
        {
        kprintf ("BUG, \"%s\"\n", info);
        }
#endif

    call_trace ();

    while (__bug_dummy_check == 0)
        {
        }
    }

