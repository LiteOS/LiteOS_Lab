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

/* ================= THIS MODULE IS FOR INTERRNAL USE ONLY! ================= */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <class.h>
#include <bug.h>

/**
 * class_init - initialize a class object
 * @class:       the class id
 * @mid:         module id
 * @obj_size:    the size of objects of the class
 * @init_rtn:    object initialization routine
 * @destroy_rtn: object destroy routine
 * @alloc_rtn:   object allocation routine
 * @free_rtn:    object free routine
 *
 * return: 0 on success, negtive value on error
 */

int class_init (class_id class, uint8_t mid, size_t obj_size,
                obj_init_pfn init_rtn, obj_destroy_pfn destroy_rtn,
                obj_alloc_pfn alloc_rtn, obj_free_pfn free_rtn)
    {
    if (unlikely (class->inited))
        {
        return 0;
        }

    class->mid         = mid;
    class->obj_size    = obj_size;
    class->init_rtn    = init_rtn;
    class->destroy_rtn = destroy_rtn;
    class->alloc_rtn   = alloc_rtn;
    class->free_rtn    = free_rtn;

    dlist_init (&class->objs);

    BUG_ON (mutex_init (&class->lock) != 0, "Fail to init class!");

    class->inited = true;

    return 0;
    }

