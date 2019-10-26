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

#include <stdlib.h>

#include <task.h>
#include <cmder.h>
#include <mempool.h>

#define TEST_ARRAY_SIZE         32

static mempool_id test_pool = NULL;

extern void context_test (uintptr_t);

static uint64_t pool_buff [100];

static volatile bool test_exit = false;

char * test_array [TEST_ARRAY_SIZE] = { NULL };

static void loop (uintptr_t arg)
    {
    int    idx;

    (void) arg;
    
    while (!test_exit)
        {
        idx = rand () & (TEST_ARRAY_SIZE - 1);
        
        if (test_array [idx] == NULL)
            {
            test_array [idx] = mempool_alloc (test_pool);
            }
        else
            {
            mempool_free (test_pool, test_array [idx]);
            test_array [idx] = NULL;
            }
        }
        
    for (idx = 0; idx < TEST_ARRAY_SIZE; idx++)
        {
        if (test_array [idx] != NULL)
            {
            mempool_free (test_pool, test_array [idx]);
            test_array [idx] = NULL;
            }
        }
        
    mempool_destroy (test_pool);
    test_pool = NULL;
    }

static int __cmd_pool_test (cmder_t * cmder, int argc, char * argv [])
    {
    if (test_pool != NULL)
        {
        test_exit = true;
        return 0;
        }

    test_exit = false;

    test_pool = mempool_create (8, 100, (char *) pool_buff);
    
    if (test_pool == NULL)
        {
        cmder_printf (cmder, "pool init fail\n");
        return -1;
        }

    task_spawn ("pool_test", 30, 0, 0x100, (int (*) (uintptr_t)) loop, 0);

    return 0;
    }

CMDER_CMD_DEF ("pool_test", "memory pool test", __cmd_pool_test);
