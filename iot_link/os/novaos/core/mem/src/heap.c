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

#include <string.h>

#include <config.h>
#include <heap.h>
#include <phys_mem.h>
#include <errno.h>
#include <class.h>
#include <init.h>
#include <warn.h>
#include <bug.h>

#ifdef CONFIG_CMDER
#include <cmder.h>
#endif

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

/* globals */

heap_t kernel_heap [1] = {0,};

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t heap_class [1];

static inline void __put_chunk (heap_t * heap, chunk_t * chunk)
    {
#ifdef CONFIG_HEAP_DEBUG

    /* chunk size not aligned */

    BUG_ON (chunk->size & ALLOC_ALIGN_MASK, "chunk size not aligned!");

    memset (chunk + 1, 0xfe, chunk->size - sizeof (chunk_t));
#endif

    __cm_put_chunk (&heap->cm, chunk);

#ifdef CONFIG_MEM_STATISTICS
    __stat_chunk_add (&heap->stat, chunk);
#endif
    }

static inline void __del_chunk (heap_t * heap, chunk_t * chunk)
    {
#ifdef CONFIG_HEAP_DEBUG

    /* chunk size not aligned */

    BUG_ON (chunk->size & ALLOC_ALIGN_MASK, "chunk size not aligned!");
#endif

    __cm_del_chunk (&heap->cm, chunk);

#ifdef CONFIG_MEM_STATISTICS
    __stat_chunk_del (&heap->stat, chunk);
#endif
    }

static inline chunk_t * __get_chunk (heap_t * heap, size_t size)
    {
    chunk_t * chunk = __cm_get_chunk (&heap->cm, size);

    if (likely (chunk != NULL))
        {
        __del_chunk (heap, chunk);

#ifdef CONFIG_HEAP_DEBUG
        memset (__get_mem_block (chunk), 0xac, chunk->size - sizeof (ach_t));
#endif
        }

    return chunk;
    }

/**
 * heap_init - initialize a heap struct
 * @heap: the given heap
 *
 * return: 0 on success, negtive value on error
 */

int heap_init (heap_t * heap)
    {
    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID; return -1,
             "Invalid heap id!");

    return obj_init (heap_class, &heap->obj);
    }

/**
 * heap_create - create a heap struct
 * @heap: the given heap
 *
 * return: the created heap, NULL on error
 */

heap_id heap_create (void)
    {

    /*
     * obj is just the first member of heap, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (heap_class), heap_t, obj);
    }

/**
 * heap_destroy - destroy a heap
 * @sem: the semaphore to be destroied
 *
 * return: 0 on success, negtive value on error
 */

int heap_destroy (heap_t * heap)
    {
    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID; return -1,
             "Invalid heap id!");

    return obj_destroy (heap_class, &heap->obj);
    }

static int __heap_add (heap_t * heap, char * buff, size_t size)
    {
    block_t * cb;
    chunk_t * chunk, * last;

    if (unlikely (mutex_lock (&heap->mux) != 0))
        {
        return -1;
        }

    /*
     * this block of memory will be initialized as:
     *
     * +----+-----+-----+-----+
     * | CB | ACH | FCH | ACH |
     * +----+-----+-----+-----+
     *
     * CB  - chunk block
     * ACH - allocated chunk header
     * FCH - free chunk header
     *
     * These two ACH at the beginning and ending are guards.
     */

    cb           = (block_t *) (buff);
    cb->size     = size;

    cb->next     = heap->blocks;
    heap->blocks = cb;

    chunk        = (chunk_t *) (cb + 1);

    chunk->prev  = NULL;
    chunk->size  = sizeof (ach_t);
    __set_chunk_allocated (chunk);

    chunk        = __get_next_chunk (chunk);
    chunk->prev  = (chunk_t *) (cb + 1);
    chunk->size  = (size - sizeof (block_t) - 2 * sizeof (ach_t));

    __put_chunk (heap, chunk);

    last         = __get_next_chunk (chunk);

    last->prev   = chunk;
    last->size   = sizeof (ach_t);
    __set_chunk_allocated (last);

    (void) mutex_unlock (&heap->mux);

    return 0;
    }

/**
 * heap_add - add a block of memory to a heap
 * @heap: the heap to add in
 * @buff: the memory address to add
 * @size: the memory block size
 *
 * return: 0 on success, negtive value on error
 */

int heap_add (heap_t * heap, char * buff, size_t size)
    {
    char * end;
    int    ret;

    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID;    return -1,
             "Invalid heap id!");

    WARN_ON (buff == NULL,
             errno = ERRNO_HEAP_ILLEGAL_BLOCK; return -1,
             "Invalid buffer!");

    WARN_ON ((uintptr_t) buff > ((uintptr_t) buff + (uintptr_t) size),
             errno = ERRNO_HEAP_ILLEGAL_BLOCK; return -1,
             "Invalid buffer!");

    end  = buff + size;

    buff = (char *) round_up   (buff, ALLOC_ALIGN);
    size = (size_t) round_down (end,  ALLOC_ALIGN) - (size_t) buff;

    WARN_ON (size < MIN_BLOCK_SIZE,
             errno = ERRNO_HEAP_ILLEGAL_BLOCK; return -1,
             "Invalid buffer");

    if (unlikely (obj_verify_protect (heap_class, &heap->obj) != 0))
        {
        return -1;
        }

#ifdef MAX_CHUNK_SIZE
    ret = -1;

    while (size >= MIN_BLOCK_SIZE)
        {
        size_t s = size > MAX_CHUNK_SIZE ? MAX_CHUNK_SIZE : size;

        if (__heap_add (heap, buff, s) != 0)
            {
            break;
            }

        ret = 0;

        size -= s;
        buff += s;
        }
#else
    ret = __heap_add (heap, buff, size);
#endif

    obj_unprotect (&heap->obj);

    return ret;
    }

/**
 * __carve_extra - round up a block of memory and carve the unused memory at
 *                 the beginning and ending
 * @heap:  the heap used
 * @chunk: the chunk which hold the memory block to round up and carve
 * @align: the alignment to round up
 *
 * return: the new aligned memory block
 */

static inline char * __carve_extra (heap_t * heap, chunk_t * chunk,
                                    size_t align, size_t size)
    {
    char    * mem         = __get_mem_block (chunk);
    char    * aligned_mem = (char *) round_up (mem, align);
    ach_t   * ach         = (ach_t *) chunk;
    chunk_t * next;
    uintptr_t usable_size;
    uintptr_t carved_size;

#ifdef CONFIG_HEAP_DEBUG

    /* memory not aligned */

    BUG_ON ((uintptr_t) mem & ALLOC_ALIGN_MASK, "memory not aligned!");
#endif

    if (aligned_mem != mem)
        {

        /*
         * carve at the begining (carve head)
         *
         *   .-chunk(FCH)-.
         *  /              \
         * +---+---+---+---+~~~+---+---+~~
         * | p | s |   l   | ~ | p | s |
         * | r | i |   i   | ~ | r | i |
         * | e | z |   s   | ~ | e | z |
         * | v | e |   t   | ~ | v | e |
         * +---+---+---+---+~~~+---+---+~~
         *         \           \       /\
         *          `-- mem     `-ach-'  `-- aligned_mem
         *
         * now "mem != aligned_mem", so there must be "chunk < ach" so there are
         * memory space in front of ach (chunk ~ ach), if "(ach - chunk) <= chunk",
         * needless create new chunk, it is not worse to add logic save such memory
         * block, because for any method to save such memory block we must allocate
         * a chunk head for it, the chunk head itself waste more memory
         */

        if ((size_t) (aligned_mem - (char *) chunk) < sizeof (chunk_t) + sizeof (ach_t))
            {
            __set_ach_to_mem (aligned_mem, chunk);
            }
        else
            {
            ach = (ach_t *) (aligned_mem - sizeof (ach_t));

            carved_size = (uintptr_t) ach - (uintptr_t) chunk;

            ach->prev   = chunk;
            ach->size   = chunk->size - carved_size;

            __get_next_chunk ((chunk_t *) ach)->prev = (chunk_t *) ach;

            chunk->size = carved_size;

            __put_chunk (heap, chunk);
            }
        }

    /*
     * carve at the ending (carve tail)
     *
     * there may be small memory gap (less than a chunk head size) betwen
     * __get_mem_block (chunk) and mem, see above "carve head" for details
     *
     *                     .-- total size is chunk->size
     *                    /
     *  ..................+....................
     * /                                       \
     * +-----+-----+----------------------------+------------+ ~ ...
     * | ACH | GAP |        usable memory       | next chunk | ~ ...
     * +-----+-----+----------------------------+------------+ ~ ...
     * \            \
     *  \            `-------- mem
     *   `-------------------- chunk
     *
     * so, the usable memory size is (chunk->size - (addr (mem) - (addr) chunk))
     *
     * we are going to carve extra memory block at the end of the usable memory
     */

    usable_size = ach->size - (uintptr_t) (aligned_mem - (char *) ach);

    carved_size = usable_size - size;

    /*
     * if the extra memory at the end is smaller than a chunk head size, do not
     * create new chunk, extra memory size is now saved in carved_size
     */

    if (carved_size >= sizeof (chunk_t))
        {

        /*
         * there is extra memory at the end of usable memory greater than chunk size:
         *
         *                              .----- usable memory
         *                             /
         *                   .........+........
         *                  /                  \
         *                 /      .- size       \
         *                /      /               \
         *               .......+......           \
         *              /              \           \
         * +-----+-----+----------------+-----------+------------+ ~ ...
         * | ACH | GAP | wanted memory  | extra mem | next chunk | ~ ...
         * +-----+-----+----------------+-----------+------------+ ~ ...
         *  \           \                 \
         *   \           \                 `-- extra memory at the end
         *    \           `------------------- mem
         *     `------------------------------ ach
         *
         * ==>
         *
         * +-----+-----+----------------+-----+-----+------------+ ~ ...
         * | ACH | GAP | wanted memory  | FCH | mem | next chunk | ~ ...
         * +-----+-----+----------------+-----+-----+------------+ ~ ...
         *  \           \                \
         *   \           \                `--- new_chunk, will return to heap
         *    \           `------------------- mem, will return to user
         *     `------------------------------ ach
         */

        ach->size  -= carved_size;

        chunk       = (chunk_t *) (aligned_mem + size);
        chunk->prev = (chunk_t *) ach;
        chunk->size = carved_size;

        next        = __get_next_chunk (chunk);

        next->prev  = chunk;

        __put_chunk (heap, chunk);
        }

#ifdef CONFIG_MEM_STATISTICS
    __stat_chunk_alloc (&heap->stat, (chunk_t *) ach);
#endif

    __set_chunk_allocated ((chunk_t *) ach);

    return aligned_mem;
    }

/**
 * heap_alloc_align - allocate a block of memory from a heap with alignment
 * @heap:  the heap to allocate from
 * @bytes: size of memory in bytes to allocate
 * @align: the expected alignment value
 *
 * return: the allocated memory block or NULL if fail
 */

char * heap_alloc_align (heap_t * heap, size_t align, size_t bytes)
    {
    chunk_t * chunk;
    char    * mem = NULL;

    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID;    return NULL,
             "Invalid heap id!");

    /* align must be power of 2 */

    WARN_ON (align & (align - 1),
             errno = ERRNO_HEAP_ILLEGAL_ALIGN; return NULL,
             "Invalid alignment!");

    align = unlikely (align < ALLOC_ALIGN) ? ALLOC_ALIGN : align;

    /* ALLOC_MIN_SIZE is defined as sizeof (dlist_t) */

    bytes = unlikely (bytes < ALLOC_MIN_SIZE) ? ALLOC_MIN_SIZE
                                              : round_up (bytes, ALLOC_ALIGN);

    if (unlikely (obj_verify_protect (heap_class, &heap->obj) != 0))
        {
        return NULL;
        }

    if (likely (mutex_lock (&heap->mux) == 0))
        {

        /* find a big enough memory chunk */

        chunk = __get_chunk (heap, bytes + sizeof (ach_t) + align - ALLOC_ALIGN);

        if (chunk != NULL)
            {
            mem = __carve_extra (heap, chunk, align, bytes);
            }

        (void) mutex_unlock (&heap->mux);
        }

    obj_unprotect (&heap->obj);

    WARN_ON (mem == NULL,
             errno = ERRNO_HEAP_NOT_ENOUGH_MEMORY,
             "Not enough memory!");

    return mem;
    }

/**
 * heap_alloc - allocate a block of memory from a heap
 * @heap:  the heap to allocate from
 * @bytes: size of memory in bytes to allocate
 *
 * return: the allocated memory block or NULL if fail
 */

char * heap_alloc (heap_t * heap, size_t bytes)
    {
    return heap_alloc_align (heap, ALLOC_ALIGN, bytes);
    }

/**
 * heap_free - free a block of memory
 * @heap:  the heap to allocate from
 *
 * return: NA
 */

void heap_free (heap_t * heap, char * mem)
    {
    chunk_t * chunk;
    chunk_t * prev_chunk;
    chunk_t * next_chunk;

    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID; return,
             "Invalid heap id!");

    /* ANSI C - free of NULL is OK */

    if (unlikely (mem == NULL))
        {
        return;
        }

    if (unlikely (obj_verify_protect (heap_class, &heap->obj) != 0))
        {
        return;
        }

    if (likely (mutex_lock (&heap->mux) == 0))
        {
        chunk = (chunk_t *) __get_ach_from_mem (mem);

        __set_chunk_free (chunk);

#ifdef CONFIG_MEM_STATISTICS
        __stat_chunk_free (&heap->stat, chunk);
#endif

        prev_chunk = __get_prev_chunk (chunk);
        next_chunk = __get_next_chunk (chunk);

        if (__is_free (prev_chunk))
            {
            __del_chunk (heap, prev_chunk);
            prev_chunk->size += chunk->size;
            chunk = prev_chunk;
            }

        if (__is_free (next_chunk))
            {
            __del_chunk (heap, next_chunk);
            chunk->size += next_chunk->size;

            next_chunk = __get_next_chunk (chunk);
            }

        next_chunk->prev = chunk;

        __put_chunk (heap, chunk);

        mutex_unlock (&heap->mux);
        }

    obj_unprotect (&heap->obj);
    }

static char * __realloc (heap_t * heap, char * ptr, size_t size)
    {
    char    * mem;
    size_t    usable_size;
    size_t    left_size;
    chunk_t * chunk;
    chunk_t * new;
    chunk_t * next;

    chunk = (chunk_t *) __get_ach_from_mem (ptr);
    next  = __get_next_chunk (chunk);

    usable_size = (size_t) (((char *) next) - ptr);

    if (usable_size < size)
        {
        mem = heap_alloc (heap, size);

        if (mem != NULL)
            {
            memcpy (mem, ptr, usable_size);
            heap_free (heap, ptr);
            }

        return mem;
        }

    /* memory in this chunk is enough, needless alloc new one */

    left_size = usable_size - size;

    if (left_size < sizeof (chunk_t))
        {
        return ptr;
        }

    /* extra memory in the end and can be released */

    new = (chunk_t *) ((char *) next - left_size);

    new->prev = chunk;

    if (mutex_lock (&heap->mux) != 0)
        {
        return ptr;
        }

    chunk->size -= left_size;

    if (__is_free (next))
        {
        new->size = left_size + next->size;
        __del_chunk (heap, next);
        next = __get_next_chunk (new);
        }
    else
        {
        new->size = left_size;
        }

    next->prev = new;

    __put_chunk (heap, new);

    (void) mutex_unlock (&heap->mux);

    return ptr;
    }

/**
 * heap_realloc - realloc memory from a heap
 * @heap: the heap to allocate from
 * @ptr:  the original memory
 * @size: the new size
 *
 * return: the allocated memory block or NULL if fail
 */

char * heap_realloc (heap_t * heap, char * ptr, size_t size)
    {
    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID; return NULL,
             "Invalid heap id!");

    if (ptr == NULL)
        {
        return heap_alloc (heap, size);
        }

    if (size == 0)
        {
        heap_free (heap, ptr);
        return NULL;
        }

    if (size < ALLOC_MIN_SIZE)
        {
        size = ALLOC_MIN_SIZE;
        }
    else
        {
        size  = round_up (size, ALLOC_ALIGN);
        }

    if (unlikely (obj_verify_protect (heap_class, &heap->obj) != 0))
        {
        return NULL;
        }

    ptr = __realloc (heap, ptr, size);

    obj_unprotect (&heap->obj);

    return ptr;
    }

#ifdef CONFIG_MEM_STATISTICS
int heap_stat_get (heap_t * heap, mem_stat_t * stat)
    {
    WARN_ON (heap == NULL,
             errno = ERRNO_HEAP_ILLEGAL_ID;   return -1,
             "Invalid heap id!");

    WARN_ON (stat == NULL,
             errno = ERRNO_HEAP_ILLEGAL_STAT; return -1,
             "Invalid stat!");

    if (unlikely (obj_verify_protect (heap_class, &heap->obj) != 0))
        {
        return -1;
        }

    if (likely (mutex_lock (&heap->mux) == 0))
        {
        *stat = heap->stat;

        stat->max_free_size = __get_max_free (&heap->cm);

        mutex_unlock (&heap->mux);
        }

    obj_unprotect (&heap->obj);

    return 0;
    }
#endif

static int __heap_init (obj_id obj, va_list valist)
    {
    heap_id heap = container_of (obj, heap_t, obj);

    memset (heap, 0, sizeof (heap_t));

    if (unlikely (__cm_init (&heap->cm) != 0))
        {
        return -1;
        }

    return mutex_init (&heap->mux);
    }

static int __heap_destroy (obj_id obj)
    {
    heap_id   heap  = container_of (obj, heap_t, obj);
    block_t * block = heap->blocks;

    /* check if there is any busy blocks in the heap */

    while (block)
        {

        /* get the first chunk and skip the first ACH */

        chunk_t * chunk = __get_next_chunk ((chunk_t *) (block + 1));

        do
            {

            /* reach last one */

            if (unlikely (chunk->size == (sizeof (ach_t) | 1)))
                {
                break;
                }

            if (unlikely (!__is_free (chunk)))
                {
                return -1;
                }

            chunk = __get_next_chunk (chunk);
            } while (1);

        block = block->next;
        }

    if (unlikely (mutex_destroy (&heap->mux) != 0))
        {
        return -1;
        }

    return 0;
    }

/**
 * heap_lib_init - heap library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int heap_lib_init (void)
    {
    const struct phys_mem * spm = system_phys_mem;
    int                     block_added = 0;

    BUG_ON (class_init (heap_class, MID_HEAP, sizeof (heap_t),
                        __heap_init, __heap_destroy, NULL, NULL) != 0,
            "fail to initialize heap_class!");

    BUG_ON (heap_init (kernel_heap) != 0, "fail to initialize kernel_heap!");

    while (spm->end)
        {
        if (heap_add (kernel_heap, spm->start, (size_t) (spm->end - spm->start)) == 0)
            {
            block_added++;
            }

        spm++;
        }

    BUG_ON (block_added == 0, "no block added!");

    return 0;
    }

MODULE_INIT (kernel, heap_lib_init);

#ifdef CONFIG_SYSCALL
static const uintptr_t syscall_entries_heap [] =
    {
    (uintptr_t) heap_create,
    (uintptr_t) heap_add,
    (uintptr_t) heap_alloc,
    (uintptr_t) heap_alloc_align,
    (uintptr_t) heap_free,
    (uintptr_t) heap_realloc,
#ifdef CONFIG_MEM_STATISTICS
    (uintptr_t) heap_stat_get,
#endif
    };

const struct syscall_table syscall_table_heap =
    {
    ARRAY_SIZE (syscall_entries_heap),
    syscall_entries_heap
    };
#endif

#ifdef CONFIG_CMDER
static inline void __dump_block (cmder_t * cmder, block_t * block)
    {
    chunk_t * chunk = (chunk_t *) (block + 1);

    cmder_printf (cmder, "\taddress    size       status\n");
    cmder_printf (cmder, "\t---------- ---------- ---------\n");

    do
        {
        cmder_printf (cmder, "\t%10p %10p %s", chunk, chunk->size & ~1u,
                      (chunk->size & 1) == 0 ? "free\n" : "allocated\n");

        if ((chunk->prev != NULL) && (chunk->size == (sizeof (ach_t) | 1)))
            {
            break;
            }

        chunk = __get_next_chunk (chunk);
        } while (1);
    }

static void __cmd_dump_heap (cmder_t * cmder, heap_t * heap, bool show_chunk)
    {
    block_t * block = heap->blocks;

    if (show_chunk)
        {
        cmder_printf (cmder, "\tchunk information:\n\n");
        }

    if (mutex_lock (&heap->mux) != 0)
        {
        return;
        }

    if (show_chunk)
        {
        while (block)
            {
            __dump_block (cmder, block);
            block = block->next;
            }
        }

#ifdef CONFIG_MEM_STATISTICS
    cmder_printf (cmder, "\n\tSUMMARY:\n\n");
    cmder_printf (cmder, "\tstatus  bytes              blocks\n");
    cmder_printf (cmder, "\t------  ------------------ ------\n");
    cmder_printf (cmder, "\tcurrent:\n");
    cmder_printf (cmder, "\tfree\t%-18p %d\n", heap->stat.free_size, heap->stat.free_chunks);
    cmder_printf (cmder, "\talloc\t%-18p %d\n", heap->stat.busy_size, heap->stat.busy_chunks);
    cmder_printf (cmder, "\tcumulative:\n");
    cmder_printf (cmder, "\tfree\t0x%-16llx %d\n", heap->stat.cum_size_freed, heap->stat.cum_freed);
    cmder_printf (cmder, "\talloc\t0x%-16llx %d\n", heap->stat.cum_size_allocated, heap->stat.cum_allocated);

    cmder_printf (cmder, "\n\tmaximum allocated size ever: %p\n", heap->stat.max_busy_size);
    cmder_printf (cmder, "\n\tmaximum free size: %p\n", __get_max_free (&heap->cm));
#endif

    mutex_unlock (&heap->mux);
    }

static int __cmd_meminfo (cmder_t * cmder, int argc, char * argv [])
    {
    const struct phys_mem * spm = system_phys_mem;
    bool                    show_chunk = false;
    int                     i;

    for (i = 1; i < argc; i++)
        {
        if (strncmp (argv [i], "--chunk", 7) == 0)
            {
            show_chunk = true;
            }
        }

    cmder_printf (cmder, "all physcial memory blocks:\n\n");

    cmder_printf (cmder, "address    end        length    \n");
    cmder_printf (cmder, "---------- ---------- ----------\n");

    while (spm->end)
        {
        cmder_printf (cmder, "%10p %10p %10p\n", spm->start, spm->end - 1,
                      spm->end - spm->start);
        spm++;
        }

    cmder_printf (cmder, "\n[kernel_heap] statistics\n\n");

    (void) __cmd_dump_heap (cmder, kernel_heap, show_chunk);

    return 0;
    }

CMDER_CMD_DEF ("meminfo", "show memory information, usage: 'meminfo [--chunk]'",
               __cmd_meminfo);

#endif
