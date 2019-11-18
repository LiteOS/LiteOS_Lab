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

uint32_t public_value = 0;

osal_mutex_t public_value_mutex;

static int user_task1_entry()
{
    while(1)
    {
        if(true == osal_mutex_lock(public_value_mutex))
        {
            printf("\r\ntask1: lock a mutex.\r\n");
            public_value += 10;
            printf("task1: public_value = %ld.\r\n", public_value);
            printf("task1: unlock a mutex.\r\n\r\n");
            osal_mutex_unlock(public_value_mutex);
            if(public_value > 100)
                break;
            
        }
    }

    return 0;
}
static int user_task2_entry()
{
    while (1)
    {
       if(true == osal_mutex_lock(public_value_mutex))
        {
            printf("\r\ntask2: lock a mutex.\r\n");
            public_value += 5; 
            printf("task2: public_value = %ld.\r\n", public_value);
            printf("task2: unlock a mutex.\r\n\r\n");
            osal_mutex_unlock(public_value_mutex);
            
            if(public_value > 90)
                break;

            osal_task_sleep(10);
        }
    }

    return 0;
}

int standard_app_demo_main()
{

    osal_mutex_create(&public_value_mutex);
    osal_task_create("user_task1",user_task1_entry,NULL,0x400,NULL,USER_TASK1_PRI);
    osal_task_create("user_task2",user_task2_entry,NULL,0x400,NULL,USER_TASK2_PRI);

    return 0;
}