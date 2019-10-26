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


#ifndef __PHYS_MEM_H__
#define __PHYS_MEM_H__

#include <stddef.h>

#include <heap.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct phys_mem
    {
    char * start;
    char * end;
    };

extern const struct phys_mem system_phys_mem [];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PHYS_MEM_H__ */

