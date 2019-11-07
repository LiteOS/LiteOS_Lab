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
#include <obj.h>
#include <irq.h>
#include <memtry.h>
#include <task.h>
#include <warn.h>
#include <critical.h>

/* macros */

/* locals */

/* inlines */

static __always_inline obj_id __obj_alloc (class_id class)
    {
    if (class->alloc_rtn != NULL)
        {
        return class->alloc_rtn ();
        }
    else
        {
        return (obj_id) malloc (class->obj_size);
        }
    }

static __always_inline void __obj_free (class_id class, obj_id obj)
    {
    if (class->free_rtn != NULL)
        {
        class->free_rtn (obj);
        }
    else
        {
        free (obj);
        }
    }

/* check if protection is not needed */

static __always_inline bool __obj_no_protect (void)
    {
    return int_context () || in_critical ();
    }

static int __obj_verify (class_id class, obj_id obj)
    {
    uintptr_t magic;

#if UINTPTR_MAX == UINT_MAX
    int       order = 2;
#endif

    if (unlikely (mem_try ((void *) &magic, (void *) &obj->magic, order)) != 0)
        {
        errno = ERRNO_OBJ_ILLEGAL_ID;
        return -1;
        }

    if (unlikely (magic != (uintptr_t) obj))
        {
        errno = ERRNO_OBJ_ILLEGAL_MAGIC;
        return -1;
        }

    if (unlikely (obj->class != class))
        {
        errno = ERRNO_OBJ_ILLEGAL_CLASS;
        return -1;
        }

    return 0;
    }

/**
 * obj_verify - verify if an object is valid
 * @class: the class this object should belongs to
 * @obj:   the object id
 *
 * can only be invoked when in "nosleep" context, if not, use obj_verify_protect
 * instead
 *
 * return: 0 on success, negtive value on error
 */

int obj_verify (class_id class, obj_id obj)
    {
#ifndef __HOST__
    WARN_ON (__obj_verify (class, obj) != 0,
             return -1,
             "Invalid object ID, errno = %p!", errno);
#endif

    return 0;
    }

/**
 * obj_verify_protect - verify if an object is valid and protect it for deletion
 * @class: the class this object should belongs to
 * @obj:   the object id
 *
 * return: 0 on success, negtive value on error
 */

int obj_verify_protect (class_id class, obj_id obj)
    {
    int ret = 0;

    /* interrupt context and critical context needless prevent for deletion */

    if (unlikely (__obj_no_protect ()))
        {
        return obj_verify (class, obj);
        }

    task_lock ();

    if (unlikely (obj_verify (class, obj) != 0))
        {
        ret = -1;
        }
    else
        {
        /* must be an object, we can write of cause */

        atomic_uint_inc (&obj->safe_cnt);
        }

    task_unlock ();

    return ret;
    }

/**
 * obj_unprotect - unprotect an object
 * @obj:   the object id
 *
 * return: NA
 */

void obj_unprotect (obj_id obj)
    {
    if (unlikely (__obj_no_protect ()))
        {
        return;
        }

    atomic_uint_dec (&obj->safe_cnt);

    if (obj->task_del != NULL && atomic_uint_get (&obj->safe_cnt) == 0)
        {
        task_resume ((task_id) obj->task_del);
        }
    }

static int __obj_init (class_id class, obj_id obj, const char * name,
                       va_list valist, unsigned int flags)
    {
    if (unlikely (class->init_rtn (obj, valist) != 0))
        {
        return -1;
        }

    obj->class    = class;
    obj->flags    = flags;
    obj->name     = name;
    obj->task_del = NULL;

    atomic_uint_set (&obj->safe_cnt, 0);

    atomic_uint_set (&obj->open_cnt, 1);

    if (unlikely (mutex_lock (&class->lock) != 0))
        {
        return -1;
        }

    dlist_add (&class->objs, &obj->node);

    obj->magic = (uintptr_t) obj;

    (void) mutex_unlock (&class->lock);

    return 0;
    }

static int __obj_eq (obj_id obj, va_list valist)
    {

    /* return -1 (fail) so the iteration will stop and return this object */

    return obj == va_arg (valist, obj_id) ? -1 : 0;
    }

static obj_id __obj_inited (class_id class, obj_id obj)
    {
    return obj_foreach (class, __obj_eq, obj);
    }

/**
 * obj_init - initialize an object
 * @class: the class this object should belongs to
 * @obj:   the object id
 *
 * return: 0 on success, negtive value on error
 */

int obj_init (class_id class, obj_id obj, ...)
    {
    int     ret;
    va_list valist;

    char  * obj_start = (char *) obj;
    char  * obj_end   = obj_start + sizeof (obj_t) - 1;

    if (__obj_verify (class, obj) == 0 && __obj_inited (class, obj) != NULL)
        {
        errno = ERRNO_OBJ_ILLEGAL_OPERATION;
        WARN ("Trying to initialize an initialized object!");
        return -1;
        }

    if (errno == ERRNO_OBJ_ILLEGAL_ID)
        {
        WARN ("Object can not be accessed!");
        return -1;
        }

    if (mem_try ((void *) obj_start, (void *) obj_start, 0) != 0 ||
        mem_try ((void *) obj_end,   (void *) obj_end,   0) != 0)
        {
        WARN ("Object can not be accessed!");
        return -1;
        }

    va_start (valist, obj);

    ret = __obj_init (class, obj, NULL, valist, OBJ_FLAGS_STATIC);

    va_end (valist);

    return ret;
    }

/**
 * obj_create - create an object
 * @class: the class this object should belongs to
 * @obj:   the object id
 *
 * return: object id on success, NULL on error
 */

obj_id obj_create (class_id class, ...)
    {
    va_list valist;
    obj_id  obj = __obj_alloc (class);

    if (unlikely (obj == NULL))
        {
        return NULL;
        }

    va_start (valist, class);

    if (unlikely (__obj_init (class, obj, NULL, valist, 0) != 0))
        {
        __obj_free (class, obj);

        obj = NULL;
        }

    va_end (valist);

    return obj;
    }

/**
 * obj_open - open a object with a given name
 * @class: the class id
 * @name:  the name of the semaphore
 * @oflag: flag, 0 or O_CREAT or O_CREAT | O_EXCL
 * @value: if oflag has O_CREAT option
 *
 * return: the allocated semaphore or NULL on error
 */

obj_id obj_open (class_id class, const char * name, int oflag, va_list valist)
    {
    obj_id obj;

    WARN_ON (name == NULL,
             errno = ERRNO_OBJ_ILLEGAL_OPERATION; return NULL,
             "Invalid name!");

    obj = obj_find (class, name);

    if (obj != NULL)
        {
        if ((oflag & O_EXCL) != 0)
            {
            errno = EEXIST;
            return NULL;
            }

        if ((obj->flags & OBJ_FLAGS_EXCL) != 0)
            {
            errno = EACCES;
            return NULL;
            }

        if (obj_verify_protect (class, obj) == 0)
            {
            atomic_uint_inc (&obj->open_cnt);
            obj_unprotect (obj);

            return obj;
            }
        }

    /* if object not exist, try to create it */

    if ((oflag & O_CREAT) == 0)
        {
        errno = ENOENT;
        return NULL;
        }

    obj = __obj_alloc (class);

    if (unlikely (obj == NULL))
        {
        return NULL;
        }

    /* eat mode, it is not supported now */

    (void) va_arg (valist, int);

    if (unlikely (__obj_init (class, obj, name, valist, 0) != 0))
        {
        __obj_free (class, obj);

        obj = NULL;
        }

    if (unlikely (((oflag & O_EXCL) != 0) && (obj != NULL)))
        {
        obj->flags |= OBJ_FLAGS_EXCL;
        }

    return obj;
    }

static int __obj_wait_unsafe (uintptr_t arg1, uintptr_t arg2)
    {
    obj_id obj = (obj_id) arg1;

    if (unlikely (atomic_uint_get (&obj->safe_cnt) != 0))
        {

        /* suspend and put current to "PEND" */

        obj->task_del = current;

        task_ready_q_del (current);

        current->status |= TASK_STATUS_PEND;
        }

    return 0;
    }

/**
 * obj_destroy - destroy an object
 * @class: the class this object should belongs to
 * @obj:   the object id
 *
 * return: 0 on success, negtive value on error
 */

int obj_destroy (class_id class, obj_id obj)
    {
    if (unlikely (mutex_lock (&class->lock) != 0))
        {
        return -1;
        }

    /*
     * objects can only be destroyed with class->lock locked, so obj_verify is
     * safe here
     */

    if (unlikely (obj_verify (class, obj) != 0))
        {
        mutex_unlock (&class->lock);
        errno = ERRNO_OBJ_ILLEGAL_ID;
        return -1;
        }

    atomic_uint_dec (&obj->open_cnt);

    /*
     * prevent the object been opened by other task after the checking of
     * open_cnt, make the checking of open_cnt and `obj->magic = 0` atomic
     */

    task_lock ();

    if (atomic_uint_get (&obj->open_cnt) != 0)
        {
        task_unlock ();
        mutex_unlock (&class->lock);

        /*
         * set errno to ERRNO_OBJ_STILL_USING saying the object is not really
         * deleted as it is still been using
         */

        errno = ERRNO_OBJ_STILL_USING;
        return 0;
        }

    /* close the door, so any access to this object is denied */

    obj->magic = 0;

    task_unlock ();

    do_critical (__obj_wait_unsafe, (uintptr_t) obj, 0);

    obj->task_del = NULL;

    /* delete object from class */

    dlist_del (&obj->node);

    mutex_unlock (&class->lock);

    if ((class->destroy_rtn != NULL) && unlikely (class->destroy_rtn (obj) != 0))
        {
        WARN ("Fail to invoke class->destroy_rtn (%p), add the object back, but it may not works fine!",
              class->destroy_rtn);

        mutex_lock (&class->lock);
        dlist_add (&class->objs, &obj->node);
        obj->magic = (uintptr_t) obj;
        atomic_uint_inc (&obj->open_cnt);
        mutex_unlock (&class->lock);

        return -1;
        }

    if ((obj->flags & OBJ_FLAGS_STATIC) == 0)
        {
        __obj_free (class, obj);
        }

    return 0;
    }

/**
 * obj_foreach - iterate all objects in a class
 * @class:       the class id
 * @foreach_rtn: the callback function for each object
 * @arg1:        the argument#1 for the callback if any
 * @arg2:        ...
 *
 * return: object last reached or NULL if all object reached
 */

obj_id obj_foreach (class_id class, obj_foreach_pfn foreach_rtn, ...)
    {
    dlist_t * itr;
    int       ret;
    va_list   valist;
    obj_id    last = NULL;

    if (unlikely (mutex_lock (&class->lock) != 0))
        {
        return NULL;
        }

    dlist_foreach (itr, &class->objs)
        {
        obj_id obj = container_of (itr, obj_t, node);

        va_start (valist, foreach_rtn);

        ret = foreach_rtn (obj, valist);

        va_end (valist);

        if (unlikely (ret != 0))
            {
            last = obj;

            break;
            }
        }

    (void) mutex_unlock (&class->lock);

    return last;
    }

static int __obj_find (obj_id obj, va_list valist)
    {
    const char * name = va_arg (valist, const char *);

    if (obj->name == NULL)
        {
        return 0;
        }

    /* return -1 so the iteration will stop and return this object */

    return strcmp (obj->name, (const char *) name) == 0 ? -1 : 0;
    }

/**
 * obj_find - find object with a given name
 * @class: the class id
 * @name:  the wanted object name
 *
 * return: the object id if found, NULL if not found
 */

obj_id obj_find (class_id class, const char * name)
    {
    return obj_foreach (class, __obj_find, name);
    }

