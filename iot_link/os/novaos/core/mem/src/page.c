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

#include <page.h>

/**
 * __page_node_put - release a page node
 */

static inline void __page_node_put (page_node_t * node)
    {
    free (node);
    }

/**
 * __page_node_get - get a page node
 */

static inline page_node_t * __page_node_get (void)
    {
    return (page_node_t *) malloc (sizeof (page_node_t));
    }

/**
 * __page_pfn_compare_nk - compare pfn node with a key
 */

static int __page_pfn_compare_nk (bi_node_t * n, uintptr_t k)
    {
    page_node_t * pn = container_of (n, page_node_t, pfn_node.bin);

    if ((uintptr_t) pn->pfn == k)
        {
        return 0;
        }

    return (uintptr_t) pn->pfn > k ? 1 : -1;
    }

/**
 * __page_pfn_compare_nn - compare two pfn nodes
 */

static int __page_pfn_compare_nn (bi_node_t * a, bi_node_t * b)
    {
    page_node_t * pb = container_of (b, page_node_t, pfn_node.bin);

    return __page_pfn_compare_nk (a, (uintptr_t) pb->pfn);
    }

/**
 * __page_siz_compare_nk - compare size node with a key
 */

static int __page_siz_compare_nk (bi_node_t * n, uintptr_t k)
    {
    page_node_t * pn = container_of (n, page_node_t, siz_node.bin);

    if ((uintptr_t) pn->pages == k)
        {
        return pn->pfn == 0 ? 0 : 1;
        }

    return (uintptr_t) pn->pages > k ? 1 : -1;
    }

/**
 * __page_siz_compare_nn - compare two size nodes
 */

static int __page_siz_compare_nn (bi_node_t * a, bi_node_t * b)
    {
    page_node_t * pa = container_of (a, page_node_t, siz_node.bin);
    page_node_t * pb = container_of (b, page_node_t, siz_node.bin);
    uint32_t      ca;
    uint32_t      cb;

    if (pa->pages == pb->pages)
        {
        ca = pa->pfn;
        cb = pb->pfn;
        }
    else
        {
        ca = pa->pages;
        cb = pb->pages;
        }

    /* impossible for ca == cb */

    return ca > cb ? 1 : -1;
    }

/**
 * page_pool_init - initialize a page pool
 * @pool: the pool need to be initialized
 */

int page_pool_init (page_pool_t * pool)
    {
    rb_init (&pool->pfn_tree, __page_pfn_compare_nn, __page_pfn_compare_nk);
    rb_init (&pool->siz_tree, __page_siz_compare_nn, __page_siz_compare_nk);

    return mutex_init (&pool->mutex);
    }

/**
 * page_pool_put - put pages into a page pool
 * @pool:  the page pool
 * @pfn:   the page frame number
 * @pages: number of pages
 */

int page_pool_put (page_pool_t * pool, uint32_t pfn, uint32_t pages)
    {
    page_node_t * node = NULL;
    page_node_t * next = NULL;
    page_node_t * prev = NULL;
    rb_node_t   * rbn;
    int           ret;

    if ((pool == NULL) || (pages == 0) || (pfn + pages < pages))
        {
        return -1;
        }

    ret = mutex_lock (&pool->mutex);

    if (ret < 0)
        {
        return ret;
        }

    rbn = rb_find_ge (&pool->pfn_tree, pfn);

    if (rbn != NULL)
        {
        next = container_of (rbn, page_node_t, pfn_node);

        if (pfn + pages > next->pfn)
            {
            ret = -1;
            goto exit;
            }
        }

    rbn = rb_find_le (&pool->pfn_tree, pfn);

    if (rbn != NULL)
        {
        prev = container_of (rbn, page_node_t, pfn_node);

        if (prev->pfn + prev->pages > pfn)
            {
            ret = -1;
            goto exit;
            }
        }

    /* try to merge next page node */

    if ((next != NULL) && (pfn + pages == next->pfn))
        {
        rb_delete (&pool->pfn_tree, &next->pfn_node);
        rb_delete (&pool->siz_tree, &next->siz_node);

        pages += next->pages;
        node   = next;
        }

    /* try to merge prev page node */

    if ((prev != NULL) && (pfn == prev->pfn + prev->pages))
        {
        rb_delete (&pool->pfn_tree, &prev->pfn_node);
        rb_delete (&pool->siz_tree, &prev->siz_node);

        pages += prev->pages;
        pfn    = prev->pfn;

        if (node == NULL)
            {
            node = prev;
            }
        else
            {
            __page_node_put (prev);
            }
        }

    if (node == NULL)
        {
        node = __page_node_get ();
        }

    node->pfn   = pfn;
    node->pages = pages;

    rb_insert (&pool->pfn_tree, &node->pfn_node);
    rb_insert (&pool->siz_tree, &node->siz_node);

exit:

    (void) mutex_unlock (&pool->mutex);

    return ret;
    }

int page_pool_get (page_pool_t * pool, uint32_t pages, uint32_t * pfn)
    {
    page_node_t * node;
    rb_node_t   * rbn;
    int           ret;

    if ((pool == NULL) || (pages == 0))
        {
        return -1;
        }

    ret = mutex_lock (&pool->mutex);

    if (ret < 0)
        {
        return ret;
        }

    rbn = rb_find_ge (&pool->siz_tree, pages);

    if (rbn == NULL)
        {
        ret = -1;
        goto exit;
        }

    node = container_of (rbn, page_node_t, siz_node);

    rb_delete (&pool->siz_tree, &node->siz_node);

    if (pages == node->pages)
        {
        *pfn = node->pfn;
        rb_delete (&pool->pfn_tree, &node->pfn_node);
        __page_node_put (node);
        }
    else
        {
        *pfn = node->pfn + (node->pages - pages);
        node->pages -= pages;
        rb_insert (&pool->siz_tree, &node->siz_node);
        }

    ret = 0;

exit:

    (void) mutex_unlock (&pool->mutex);

    return ret;
    }

