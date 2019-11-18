/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
/**
 *  DATE          AUTHOR         INSTRUCTION
 *  2019-11-16    mculover666    The first version  
 *
 */
#include <osal.h>

#define USER_TASK1_PRI  12
#define USER_TASK2_PRI  11

uint32_t user_task1_id = 0;
uint32_t user_task2_id = 0;

static int user_task1_entry()
{
    int n = 0;

    for(n = 0; n < 5; n++)
    {
        printf("task1: my task id is %ld, n = %d!\r\n", user_task1_id, n++);
        osal_task_sleep(2*1000);
    
    }

    printf("user task 1 exit!\r\n");
    return 0;
}
static int user_task2_entry()
{
    while (1)
    {
        printf("task 2: my task id is %ld!\r\n", user_task2_id);
        osal_task_sleep(2*1000);
    }
}

int standard_app_demo_main()
{
    user_task1_id = osal_task_create("user_task1",user_task1_entry,NULL,0x400,NULL,USER_TASK1_PRI);
    user_task2_id = osal_task_create("user_task2",user_task2_entry,NULL,0x400,NULL,USER_TASK2_PRI);

    return 0;
}
