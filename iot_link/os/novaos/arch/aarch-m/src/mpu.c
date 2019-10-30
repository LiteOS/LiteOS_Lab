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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <compiler.h>
#include <task.h>
#include <heap.h>
#include <init.h>
#include <syscall.h>
#include <warn.h>

#include <arch/mpu.h>
#include <arch/regset.h>
#include <arch/sync.h>

/* macros */

/* MPU_TYPE */

#define MPU_TYPE_IREGION_SHIFT      (16)
#define MPU_TYPE_IREGION_MASK       (0xff << MPU_TYPE_IREGION_SHIFT)
#define MPU_TYPE_DREGION_SHIFT      (8)
#define MPU_TYPE_DREGION_MASK       (0xff << MPU_TYPE_DREGION_SHIFT)
#define MPU_TYPE_SEPARATE           (1)

/* MPU_CTRL */

#define MPU_CTRL_PRIVDEFENA         (1 << 2)
#define MPU_CTRL_HFNMIENA           (1 << 1)
#define MPU_CTRL_ENABLE             (1 << 0)

/* MPU_RNR */

#define MPU_CTRL_RNR_SHIFT          (0)
#define MPU_CTRL_RNR_MASK           (0xff)

/* MPU_RBAR */

#define MPU_RBAR_REGION_SHIFT       (0)
#define MPU_RBAR_REGION_MASK        (0xf)
#define MPU_RBAR_VALID              (1 << 4)
#define MPU_RBAR_ADDR_SHIFT         (5)
#define MPU_RBAR_ADDR_MASK          (0xffffffe0)

/* MPU_RASR */

#define MPU_RASR_ENABLE             (1 << 0)
#define MPU_RASR_SIZE_SHIFT         (1)
#define MPU_RASR_SIZE_MASK          (0x1f << MPU_RASR_SIZE_SHIFT)
#define MPU_RASR_SRD_SHIFT          (8)
#define MPU_RASR_SRD_MASK           (0xff << MPU_RASR_SRD_SHIFT)
#define MPU_RASR_B                  (1 << 16)
#define MPU_RASR_C                  (1 << 17)
#define MPU_RASR_S                  (1 << 18)
#define MPU_RASR_TEX_SHIFT          (19)
#define MPU_RASR_TEX_MASK           (7 << MPU_RASR_TEX_SHIFT)
#define MPU_RASR_AP_SHIFT           (24)
#define MPU_RASR_AP_MASK            (3 << MPU_RASR_AP_SHIFT)
#define MPU_RASR_XN                 (1 << 28)

/* all bits can not be set by users */

#define MPU_RASR_USER_RESERVED      (0xe8c0fffe)

/* typedefs */

struct mpu
    {
    volatile uint32_t type;
    volatile uint32_t ctrl;
    volatile uint32_t rnr;
    volatile uint32_t rbar;
    volatile uint32_t rasr;
#ifdef CONFIG_AARCH_M_MAINLINE
    volatile uint32_t rbar_a1;
    volatile uint32_t rasr_a1;
    volatile uint32_t rbar_a2;
    volatile uint32_t rasr_a2;
    volatile uint32_t rbar_a3;
    volatile uint32_t rasr_a3;
#endif
    };

/* locals */

static bool               mpu_supported = false;
static struct mpu * const mpu = (struct mpu *) 0xe000ed90;
static unsigned int       mpu_regions;
static int                mpu_tls_slot;

/* inlines */

static __always_inline uint32_t __rasr_size_calc (uint32_t size)
    {
    return (uint32_t) ((32 - __clz_u32 (size - 1) - 1) << 1);
    }

static inline int __verify_region (uint32_t addr, uint32_t size, uint32_t attr)
    {

    /* check size is power of 2 */

    WARN_ON ((size & (size - 1)) != 0,
             return -1,
             "MPU region (%08x %08x %08x) invlaid, size not aligned!", addr, size, attr);

    /* check addr is aligned at size */

    WARN_ON ((addr & (size - 1)) != 0,
             return -1,
             "MPU region (%08x %08x %08x) invlaid, addr not aligned!", addr, size, attr);

    /* check if any reserved bit in attribute */

    WARN_ON ((attr & MPU_RASR_USER_RESERVED) != 0,
             return -1,
             "MPU region (%08x %08x %08x) invlaid, attr invalid!", addr, size, attr);

    return 0;
    }

static inline int __verify_regions (struct mpu_region * regions)
    {
    unsigned int nr_regions = 0;

    while (regions->size != 0)
        {
        if (unlikely (__verify_region (regions->addr, regions->size, regions->attr) != 0))
            {
            return -1;
            }

        nr_regions++;
        regions++;
        }

    /* can not "==", leave one entry for stack */

    WARN_ON (nr_regions >= mpu_regions,
             return -1,
             "Too many regions!");

    return 0;
    }

static void __mpu_task_switch_hook (task_id old, task_id new)
    {
    struct mpu_entry * mpu_ctx = NULL;
    unsigned int       i;

    (void) old;

    task_tls_get (new, mpu_tls_slot, (uintptr_t *) &mpu_ctx);

    if (mpu_ctx == NULL)
        {
        return;
        }

    for (i = 0; i < mpu_regions; i++)
        {
        if (mpu_ctx [i].rbar == 0)
            {
            break;
            }

        mpu->rbar = mpu_ctx [i].rbar;
        mpu->rasr = mpu_ctx [i].rasr;
        }

    dsb ();
    isb ();
    }

/**
 * mpu_task_spawn - spawn a task with mpu protect
 * @name:       the name of the task being created
 * @prio:       the priority of the task
 * @options:    options
 * @stack_size: the stack size the task required
 * @entry:      the entry point of the task
 * @arg:        the argument of the task
 * @regions:    the memory regions for this task
 *
 * return: the created task handler or NULL of fail
 */

task_id mpu_task_spawn (const char * name, uint8_t prio, uint32_t options,
                        size_t stack_size, int (* entry) (uintptr_t),
                        uintptr_t arg, struct mpu_region * regions)
    {
    size_t             align;
    task_id            task;
    char             * stack;
    struct mpu_entry * mpu_ctx;
    uint32_t           region;

    WARN_ON (!mpu_supported,
             errno = ERRNO_MPU_ILLEGAL_OPERATION; return NULL,
             "MPU not supported!");

    WARN_ON (regions == NULL,
             errno = ERRNO_MPU_ILLEGAL_REGIONS; return NULL,
             "Invalid MPU regions!");

    if (unlikely (__verify_regions (regions)))
        {
        errno = ERRNO_MPU_ILLEGAL_REGIONS;
        return NULL;
        }

    stack_size = stack_size_get (stack_size + sizeof (uint64_t));

    align = 1 << (32 - __clz_u32 ((uint32_t) stack_size));

    stack_size = round_up (stack_size, align >> 3);

    task = malloc (sizeof (task_t) + sizeof (struct mpu_entry) * mpu_regions);

    if (unlikely (task == NULL))
        {
        return NULL;
        }

    mpu_ctx = (struct mpu_entry *) (task + 1);

    memset (mpu_ctx, 0, sizeof (struct mpu_entry) * mpu_regions);

    stack = heap_alloc_align (kernel_heap, align, stack_size);

    if (unlikely (stack == NULL))
        {
        free (task);
        return NULL;
        }

    if (task_init (task, stack, name, prio, options, stack_size, entry, arg) != 0)
        {
        free (task);
        free (stack);
        return NULL;
        }

    /* set to unprivileged */

    __regset_set_user (&task->regset);

    /* install MPU context */

    task_tls_set (task, mpu_tls_slot, (uintptr_t) mpu_ctx);

    /* install stack in mpu_entry */

    region = 0;

    mpu_ctx->rbar = MPU_RBAR_VALID | (uint32_t) stack | region;
    mpu_ctx->rasr = MPU_ATTR_RW_RW_ | MPU_ATTR_WB | __rasr_size_calc (align) |
                    MPU_RASR_ENABLE;

    /*
     * stack_size is:
     *
     *      (x / 8) * align
     *
     * rasr.srd should be:
     *
     *       x zeros  8 - x ones
     *      0 0 ... 0 1 1 ... 1
     *
     * 8 - x ones with leading zeros is just equal to:
     *
     *      (1 << (8 - x)) - 1
     *
     * and x here is just equal to:
     *
     *      stack_size / (align >> 3)
     *
     * and this works fine even if x == 8 (align == stack_size)
     */

    mpu_ctx->rasr |= ((1u << (8 - stack_size / (align >> 3))) - 1) << MPU_RASR_SRD_SHIFT;

    /* install user claimed regions */

    while (regions->size != 0)
        {
        mpu_ctx++;
        region++;

        mpu_ctx->rbar = regions->addr | MPU_RBAR_VALID | region;
        mpu_ctx->rasr = regions->attr | __rasr_size_calc (regions->size) | MPU_RASR_ENABLE;

        regions++;
        }

    if (unlikely (task_resume (task) != 0))
        {
        task_delete (task);

        task = NULL;
        }

    return task;
    }

int mpu_region_add (task_id task, uint32_t addr, uint32_t size, uint32_t attr)
    {
    struct mpu_entry * mpu_ctx;
    unsigned int       i;

    if (unlikely (task_tls_get (task, mpu_tls_slot, (uintptr_t *) &mpu_ctx) != 0))
        {
        return -1;
        }

    WARN_ON (mpu_ctx == NULL,
             errno = ERRNO_MPU_ILLEGAL_OPERATION; return -1,
             "MPU context invalid!");

    if (unlikely (__verify_region (addr, size, attr) != 0))
        {
        errno = ERRNO_MPU_ILLEGAL_REGIONS;
        return -1;
        }

    for (i = 0; i < mpu_regions; i++)
        {
        if (mpu_ctx [i].rbar != 0)
            {
            continue;
            }

        mpu_ctx [i].rbar = addr | MPU_RBAR_VALID | i;
        mpu_ctx [i].rasr = attr | __rasr_size_calc (size);

        if (task == current)
            {
            task_lock ();
            mpu->rbar = mpu_ctx [i].rbar;
            mpu->rasr = mpu_ctx [i].rasr;
            task_unlock ();
            }

        return 0;
        }

    WARN ("No more entries!");

    return -1;
    }

/**
 * mpu_init - mpu module initialization
 *
 * return: 0, or negtive value if error
 */

static int mpu_init (void)
    {
    mpu_regions = (mpu->type & MPU_TYPE_DREGION_MASK) >> MPU_TYPE_DREGION_SHIFT;

    WARN_ON (mpu_regions == 0,
             errno = ERRNO_MPU_UNAVAILABLE; return -1,
             "MPU not supported!");

    /*
     * the background map region attributes for privileged only:
     *
     * 0x00000000 ~ 0x1fffffff XRW WT       ROM/FLASH
     * 0x20000000 ~ 0x3fffffff XRW WB       SRAM
     * 0x40000000 ~ 0x5fffffff -RW device   on-chip peripheral
     * 0x60000000 ~ 0x7fffffff XRW WB       RAM
     * 0x80000000 ~ 0x9fffffff -RW WT       RAM
     * 0xa0000000 ~ 0xbfffffff -RW device   shared device
     * 0xc0000000 ~ 0xdfffffff -RW device   non-shared device
     * 0xe0000000 ~ 0xe00fffff -RW strongly PPB
     * 0xe0100000 ~ 0xffffffff -RW device   vendor system region
     *
     * refer to ValidateAddress, DefaultMemoryAttributes and DefaultPermissions
     * in DDI0419C and DDI0403E
     *
     * the default background map is just fine for privileged regions so only
     * use MPU entries for unprivileged tasks
     */

    mpu->ctrl = MPU_CTRL_PRIVDEFENA | MPU_CTRL_ENABLE;

    mpu_tls_slot = task_tls_slot_alloc ();

    WARN_ON (mpu_tls_slot == -1,
             return -1,
             "Fail to allocate TLS slot!");

    WARN_ON (task_switch_hook_add (__mpu_task_switch_hook) == -1,
             return -1,
             "Fail to add task switch hook!");

    mpu_supported = true;

    return 0;
    }

MODULE_INIT (postkernel, mpu_init);

static const uintptr_t syscall_entries_mpu [] =
    {
    (uintptr_t) mpu_task_spawn,
    (uintptr_t) mpu_region_add,
    };

const struct syscall_table syscall_table_mpu =
    {
    ARRAY_SIZE (syscall_entries_mpu),
    syscall_entries_mpu
    };

