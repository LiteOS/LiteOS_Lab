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

#ifndef __LIST_H__
#define __LIST_H__

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* double list */

typedef struct dlist
    {
    struct dlist * prev, * next;
    } dlist_t;

/* macros */

/**
 * DLIST_INIT - create a empty list head body
 */

#define DLIST_INIT(h)           { &(h), &(h) }

/**
 * DLIST_DEF - define a empty list head
 */

#define DLIST_DEF(name)         dlist_t name = DLIST_INIT (name)

/* inlines */

/* list operations */

/**
 * dlist_init - initialize a list head
 * @head: the list head to initialize
 *
 * return: NA
 */

static __always_inline void dlist_init (dlist_t * head)
    {
    head->prev = head->next = head;
    }

/**
 * __dlist_add_at - insert a new entry between two known consecutive entries
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */

static __always_inline void __dlist_add_at (dlist_t * prev, dlist_t * next, dlist_t * node)
    {
    node->prev = prev;
    node->next = next;
    prev->next = node;
    next->prev = node;
    }

/**
 * dlist_add - add a new entry at the front of the list
 * @head: list head to add in
 * @node: the node to be added
 *
 * Insert a new entry after the specified head
 * This is good for implementing stacks
 */

static __always_inline void dlist_add (dlist_t * head, dlist_t * node)
    {
    __dlist_add_at (head, head->next, node);
    }

/**
 * dlist_add_tail - add a new entry at the tail of the list
 * @head: list head to add in
 * @node: the node to be added
 *
 * Insert a new entry before the specified head
 * This is useful for implementing queues
 */

static __always_inline void dlist_add_tail (dlist_t * head, dlist_t * node)
    {
    __dlist_add_at (head->prev, head, node);
    }

/**
 * dlist_del - deletes entry from list.
 * @node: the node to be deleted
 *
 * Note: dlist_empty () on the node does not return true after this, the entry is
 * in an undefined state
 */

static __always_inline void dlist_del (dlist_t * node)
    {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    }

/**
 * dlist_del - deletes entry from list and init it.
 * @node: the node to be deleted
 *
 * Note: dlist_empty () on the node return true after this
 */

static __always_inline void dlist_del_init (dlist_t * node)
    {
    dlist_del  (node);
    dlist_init (node);
    }

/**
 * dlist_empty - check if a list is empty
 * @head: the list head to check
 */

static __always_inline int dlist_empty (dlist_t * head)
    {
    return head->prev == head;
    }

/**
 * __dlist_foreach - iterate over a list
 * @pos:  the &dlist_t to use as a loop cursor
 * @head: the head for your list
 * @dir:  iterate direction, next for forward, prev for backward
 */

#define __dlist_foreach(pos, head, dir)                     \
    for (pos = (head)->dir; pos != (head); pos = pos->dir)

/**
 * dlist_foreach - iterate over a list
 * @pos:  the &dlist_t to use as a loop cursor
 * @head: the head for your list
 */

#define dlist_foreach(pos, head)                            \
    __dlist_foreach (pos, head, next)

/**
 * dlist_foreach_backward - iterate over a list backwards
 * @pos:  the &dlist_t to use as a loop cursor
 * @head: the head for your list
 */

#define dlist_foreach_backward(pos, head)                   \
    __dlist_foreach (pos, head, prev)

/**
 * __dlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:        the &struct list_head to use as a loop cursor
 * @n:          another &struct list_head to use as temporary storage
 * @head:       the head for your list
 * @dir:    iterate direction, next for forward, prev for backward
 */

#define __dlist_foreach_safe(pos, n, head, dir)             \
    for (pos = (head)->dir, n = pos->dir; pos != (head);    \
         pos = n, n = n->dir)

/**
 * dlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:        the &struct list_head to use as a loop cursor
 * @n:          another &struct list_head to use as temporary storage
 * @head:       the head for your list
 */

#define dlist_foreach_safe(pos, n, head)                    \
    __dlist_foreach_safe (pos, n, head, next)

/**
 * list_for_each_safe_backward - iterate over a list safe against removal
 *                               of list entry backwards
 * @pos:        the &struct list_head to use as a loop cursor
 * @n:          another &struct list_head to use as temporary storage
 * @head:       the head for your list
 */

#define dlist_foreach_safe_backward(pos, n, head)           \
    __dlist_foreach_safe (pos, n, head, prev)

/* single list */

typedef struct slist
    {
    struct slist * next;
    } slist_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIST_H__ */

