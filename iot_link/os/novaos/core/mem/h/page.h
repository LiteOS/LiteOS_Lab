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

#ifndef __PAGE_H__
#define __PAGE_H__

#include <mutex.h>

#include <rbtree.h>
#include <list.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct page_node
    {
    uint32_t  pfn;
    uint32_t  pages;
    rb_node_t pfn_node;
    rb_node_t siz_node;
    } page_node_t;

typedef struct page_pool
    {
    rb_tree_t pfn_tree;
    rb_tree_t siz_tree;
    mutex_t   mutex;
    } page_pool_t;

extern int page_pool_init (page_pool_t * pool);
extern int page_pool_put  (page_pool_t * pool, uint32_t pfn, uint32_t pages);
extern int page_pool_get  (page_pool_t * pool, uint32_t pages, uint32_t * pfn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PAGE_H__ */

