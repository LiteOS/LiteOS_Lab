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

#ifndef __AARCH_M_REGSET_H__
#define __AARCH_M_REGSET_H__

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define CONTROL_nPRIV_PRIVILEGED    0
#define CONTROL_nPRIV_UNPRIVILEGED  1
#define CONTROL_SPSEL_PSP           2
#define CONTROL_nPRIV_FPCA          4

#define EXC_RETURN_BASIC            0xfffffffd
#define EXC_RETURN_EXTENDED         0xffffffed

#define EXC_RETURN_EXTENDED_MASK    0x00000010

/*
 * exception stack point frame saved by the processor
 */

struct esp
    {
    uint32_t     r0;
    uint32_t     r1;
    uint32_t     r2;
    uint32_t     r3;
    uint32_t     ip;
    uint32_t     lr;
    uint32_t     pc;
    uint32_t     epsr;
    };

#ifdef CONFIG_FPU
struct esp_extended
    {
    uint64_t     d0;
    uint64_t     d1;
    uint64_t     d2;
    uint64_t     d3;
    uint64_t     d4;
    uint64_t     d5;
    uint64_t     d6;
    uint64_t     d7;
    uint32_t     fpscr;
    uint32_t     reserved;
    };
#endif

struct regset
    {
    uint32_t     r4;
    uint32_t     r5;
    uint32_t     r6;
    uint32_t     r7;
    uint32_t     r8;
    uint32_t     r9;
    uint32_t     r10;
    uint32_t     r11;
    struct esp * sp;
    uint32_t     control;
#ifdef CONFIG_FPU
    uint32_t     exc_return;
    uint32_t     pad;
    uint64_t     d8;
    uint64_t     d9;
    uint64_t     d10;
    uint64_t     d11;
    uint64_t     d12;
    uint64_t     d13;
    uint64_t     d14;
    uint64_t     d15;
#endif
    /* add pad for 8-bytes alignment if needed */
    };

#define EPSR_T              (1 << 24)

/**
 * __regset_set_rv - set the return value in a register set
 * @regset: the register set struct
 * @retval: the new return value
 *
 * return: NA
 */

static inline void __regset_set_rv (struct regset * regset, int retval)
    {
    regset->sp->r0 = (uint32_t) retval;
    }

/**
 * __regset_set_pc - set the program counter in a register set
 * @regset: the register set struct
 * @pc:     the new program counter
 *
 * return: NA
 */

static inline void __regset_set_pc (struct regset * regset, uintptr_t pc)
    {
    regset->sp->pc = (uint32_t) pc;
    }

/**
 * __regset_set_lr - set the link regiseter (return address) in a register set
 * @regset: the register set struct
 * @lr:     the new link register
 *
 * return: NA
 */

static inline void __regset_set_lr (struct regset * regset, uintptr_t lr)
    {
    regset->sp->lr = (uint32_t) lr;
    }

/**
 * __regset_get_pc - get the program counter from a register set
 * @regset: the register set struct
 *
 * return: NA
 */

static inline uintptr_t __regset_get_pc (struct regset * regset)
    {
    return (uintptr_t) regset->sp->pc;
    }

/**
 * __regset_get_sp - get the stack pointer from a register set
 * @regset: the register set struct
 *
 * return: NA
 */

static inline uintptr_t __regset_get_sp (struct regset * regset)
    {
    return (uintptr_t) regset->sp;
    }

/**
 * __regset_set_arg - set the argument in a register set
 * @regset: the register set struct
 * @argn:   which argument will be set, must be less than 4
 * @arg:    the argument value
 *
 * return: NA
 */

static inline void __regset_set_arg (struct regset * regset, unsigned int argn,
                                     uintptr_t arg)
    {
    uint32_t * pa = &regset->sp->r0;

    if (argn > 4)
        {
        return;
        }

    pa [argn] = (uint32_t) arg;
    }

/**
 * __regset_init - initialize a register set
 * @regset: the register set struct
 * @stack:  the initial stack base
 *
 * return: NA
 */

static inline void __regset_init (struct regset * regset, char * stack)
    {
    struct esp * sp = ((struct esp *) stack) - 1;

    memset (regset, 0, sizeof (struct regset));
    memset (sp,     0, sizeof (struct esp));

    sp->epsr           = EPSR_T;
    regset->sp         = sp;
    regset->control    = CONTROL_nPRIV_PRIVILEGED;
#ifdef CONFIG_FPU
    regset->exc_return = EXC_RETURN_BASIC;
#endif
    }

/**
 * __regset_set_user - set a regset to user (unprivileged)
 * @regset: the register set struct
 *
 * return: NA
 */

static inline void __regset_set_user (struct regset * regset)
    {
    regset->control = CONTROL_nPRIV_UNPRIVILEGED;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_REGSET_H__ */

