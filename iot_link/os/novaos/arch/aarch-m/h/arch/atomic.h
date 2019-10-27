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

#ifndef __AARCHM_ATOMIC_H__
#define __AARCHM_ATOMIC_H__

#include <stdbool.h>

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* inlines */

/**
 * atomic_int_get - get the value of an atomic_int variable
 * @a: address of the atomic_int variable
 *
 * return: value of the atomic_int variable
 */

static __always_inline int atomic_int_get (atomic_int * a)
    {
    return a->val;
    }

/**
 * atomic_uint_get - get the value of an atomic_uint variable
 * @a: address of the atomic_uint variable
 *
 * return: value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_get (atomic_uint * a)
    {
    return a->val;
    }

/**
 * atomic_int_set - set the value of an atomic_int variable
 * @a: address of the atomic_int variable
 *
 * return: NA
 */

static __always_inline void atomic_int_set (atomic_int * a, int v)
    {
    a->val = v;
    }

/**
 * atomic_uint_set - set the value of an atomic_uint variable
 * @a: address of the atomic_int variable
 *
 * return: NA
 */

static __always_inline void atomic_uint_set (atomic_uint * a, unsigned int v)
    {
    a->val = v;
    }

/* macro to generate math operation atomic asm code block */

#ifdef CONFIG_AARCH_M_EXCLUSIVE
#define ATOMIC_MATH_ASM(inst, old)                                              \
__asm__ __volatile__ (                                                          \
"0:     ldrex   %0, [%1]                                                     \n"\
"     " inst "  lr, %0, %2                                                   \n"\
"       strex   r3, lr, [%1]                                                 \n"\
"       cbz     r3, 1f                  @ rd = 0 if the operation success    \n"\
"       b       0b                                                           \n"\
"1:                                                                          \n"\
        : "=&r" (old)                                                           \
        : "r" (a), "r" (v)                                                      \
        : "lr", "cc", "memory", "r3"                                            \
        )
#else
#define ATOMIC_MATH_ASM(inst, old)                                              \
__asm__ __volatile__ (                                                          \
"       mrs     lr, primask                                                  \n"\
"       cpsid   i                                                            \n"\
"       ldr     %0, [%1]                                                     \n"\
"     " inst "  %2, %0                                                       \n"\
"       str     %2, [%1]                                                     \n"\
"       msr     primask, lr                                                  \n"\
        : "=&r" (old)                                                           \
        : "r" (a), "r" (v)                                                      \
        : "lr", "cc", "memory"                                                  \
        )
#endif

/* macro to generate math operation atomic routines */

#define ATOMIC_MATH(inst, type)                                                 \
do                                                                              \
    {                                                                           \
    type old;                                                                   \
    ATOMIC_MATH_ASM (#inst, old);                                               \
    return old;                                                                 \
    } while (0)

/**
 * atomic_int_add - atomic add value to an atomic_int variable
 * @a: address of the atomic_int variable
 * @v: the value to be added
 *
 * return: the original value of the atomic_int variable
 */

static __always_inline int atomic_int_add (atomic_int * a, int v)
    {
    ATOMIC_MATH (add, int);
    }

/**
 * atomic_uint_add - atomic add value to an atomic_uint variable
 * @a: address of the atomic_uint variable
 * @v: the value to be added
 *
 * return: the original value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_add (atomic_uint * a, unsigned int v)
    {
    ATOMIC_MATH (add, unsigned int);
    }

/**
 * atomic_int_sub - atomic sub value from an atomic_int variable
 * @a: address of the atomic_int variable
 * @v: the value to sub
 *
 * return: the original value of the atomic_int variable
 */

static __always_inline int atomic_int_sub (atomic_int * a, int v)
    {
    ATOMIC_MATH (sub, int);
    }

/**
 * atomic_uint_sub - atomic sub value from an atomic_uint variable
 * @a: address of the atomic_uint variable
 * @v: the value to sub
 *
 * return: the original value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_sub (atomic_uint * a, unsigned int v)
    {
    ATOMIC_MATH (sub, unsigned int);
    }

/**
 * atomic_int_and - atomic bitwise and value with an atomic_int variable
 * @a: address of the atomic_int object
 * @v: the value to sub
 *
 * return: the original value of the atomic_int variable
 */

static __always_inline int atomic_int_and (atomic_int * a, int v)
    {
    ATOMIC_MATH (and, int);
    }

/**
 * atomic_uint_and - atomic bitwise and value with an atomic_uint variable
 * @a: address of the atomic_uint object
 * @v: the value to sub
 *
 * return: the original value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_and (atomic_uint * a, unsigned int v)
    {
    ATOMIC_MATH (and, unsigned int);
    }

/**
 * atomic_int_orr - atomic bitwise orr value with an atomic_int variable
 * @a: address of the atomic_int variable
 * @v: the value to sub
 *
 * return: the original value of the atomic_int variable
 */

static __always_inline int atomic_int_orr (atomic_int * a, int v)
    {
    ATOMIC_MATH (orr, int);
    }

/**
 * atomic_uint_orr - atomic bitwise orr value with an atomic_uint variable
 * @a: address of the atomic_uint variable
 * @v: the value to sub
 *
 * return: the original value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_orr (atomic_uint * a, unsigned int v)
    {
    ATOMIC_MATH (orr, unsigned int);
    }

/**
 * atomic_int_xor - atomic bitwise xor value with an atomic_int variable
 * @a: address of the atomic_int object
 * @v: the value to sub
 *
 * return: the original value of the atomic_int variable
 */

static __always_inline int atomic_int_xor (atomic_int * a, int v)
    {
    ATOMIC_MATH (xor, int);
    }

/**
 * atomic_uint_xor - atomic bitwise xor value with an atomic_uint variable
 * @a: address of the atomic_uint object
 * @v: the value to sub
 *
 * return: the original value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_xor (atomic_uint * a, unsigned int v)
    {
    ATOMIC_MATH (xor, unsigned int);
    }

/**
 * atomic_int_inc - increase an atomic_int variable by 1
 * @a: address of the atomic_int variable
 *
 * return: the original value of the atomic variable
 */

static __always_inline int atomic_int_inc (atomic_int * a)
    {
    return atomic_int_add (a, 1);
    }

/**
 * atomic_uint_inc - increase an atomic_uint variable by 1
 * @a: address of the atomic_uint variable
 *
 * return: the original value of the atomic variable
 */

static __always_inline unsigned int atomic_uint_inc (atomic_uint * a)
    {
    return atomic_uint_add (a, 1);
    }

/**
 * atomic_int_dec - decrease an atomic_int variable by 1
 * @a: address of the atomic_int object
 *
 * return: the original value of the atomic_int variable
 */

static __always_inline int atomic_int_dec (atomic_int * a)
    {
    return atomic_int_add (a, -1);
    }

/**
 * atomic_uint_dec - decrease an atomic_uint variable by 1
 * @a: address of the atomic_uint object
 *
 * return: the original value of the atomic_uint variable
 */

static __always_inline unsigned int atomic_uint_dec (atomic_uint * a)
    {
    return atomic_uint_add (a, (unsigned int) -1);
    }

/**
 * atomic_int_set_eq - set the value (v) of a atomic_int object if equal its value
 *                     equal to (o)
 * @a: address of the atomic_int object
 * @o: old value expected
 * @v: new value to set
 *
 * return: true if the set is actually executed, FALSE otherwise
 */

static __always_inline bool atomic_int_set_eq (atomic_int * a, int o, int v)
    {
    bool ret;

    __asm__ __volatile__
        (
#ifdef CONFIG_AARCH_M_EXCLUSIVE
"       mov     %0, #1                                                       \n"
"0:     ldrex   r3, [%1]                                                     \n"
"       cmp     r3, %2                                                       \n"
"       bne     1f                                                           \n"
"       strex   r3, %3, [%1]                                                 \n"
"       cbz     r3, 2f                  @ rd = 0 if the operation success    \n"
"       b       0b                                                           \n"
"1:     clrex                                                                \n"
"       mov     %0, #0                                                       \n"
"2:                                                                          \n"
#else
"       mrs     lr, primask                                                  \n"
"       cpsid   i                                                            \n"
"       ldr     r3, [%1]                                                     \n"
"       cmp     r3, %2                                                       \n"
"       bne     0f                                                           \n"
"       str     %3, [%1]                                                     \n"
"       msr     primask, lr                                                  \n"
"       movs    %0, #1                                                       \n"
"       b       1f                                                           \n"
"0:     msr     primask, lr                                                  \n"
"       movs    %0, #0                                                       \n"
"1:                                                                          \n"
#endif
        : "=&r" (ret)
        : "r" (a), "r" (o), "r" (v)
        : "lr", "cc", "memory", "r3"
        );

    return ret;
    }

/**
 * atomic_uint_set_eq - set the value (v) of a atomic_uint object if its value
 *                      equal to (o)
 * @a: address of the atomic_uint object
 * @o: old value expected
 * @v: new value to set
 *
 * return: true if the set is actually executed, FALSE otherwise
 */

static __always_inline bool atomic_uint_set_eq (atomic_uint * a, int o, int v)
    {
    return atomic_int_set_eq ((atomic_int *) a, (int) o, (int) v);
    }

/**
 * atomic_int_add_ne - add the value (v) to a atomic_int object if its value not
 *                     equal to (o)
 * @a: address of the atomic_int object
 * @o: old value expected
 * @v: new value to add
 *
 * return: true if the value is added
 */

static __always_inline bool atomic_int_add_ne (atomic_int * a, int o, int v)
    {
    bool ret;

    __asm__ __volatile__
        (
#ifdef CONFIG_AARCH_M_EXCLUSIVE
#define ATOMIC_ADD_NE_ASM
"       mov     %0, #1                                                       \n"
"0:     ldrex   r3, [%1]                                                     \n"
"       cmp     r3, %2                                                       \n"
"       beq     1f                                                           \n"
"       add     lr, r3, %3                                                   \n"
"       strex   r3, lr, [%1]                                                 \n"
"       cbz     r3, 2f                  @ rd = 0 if the operation success    \n"
"       b       0b                                                           \n"
"1:     clrex                                                                \n"
"       mov     %0, #0                                                       \n"
"2:                                                                          \n"
#else
#define ATOMIC_ADD_NE_ASM
"       mrs     lr, primask                                                  \n"
"       cpsid   i                                                            \n"
"       ldr     r3, [%1]                                                     \n"
"       cmp     r3, %2                                                       \n"
"       beq     0f                                                           \n"
"       add     r3, r3, %3                                                   \n"
"       str     r3, [%1]                                                     \n"
"       msr     primask, lr                                                  \n"
"       movs    %0, #1                                                       \n"
"       b       1f                                                           \n"
"0:     msr     primask, lr                                                  \n"
"       movs    %0, #0                                                       \n"
"1:                                                                          \n"
#endif
        : "=&r" (ret)
        : "r" (a), "r" (o), "r" (v)
        : "lr", "cc", "memory", "r3"
        );

    return ret;
    }

/**
 * atomic_uint_add_ne - add the value (v) to a atomic_uint object if its value not
 *                      equal to (o)
 * @a: address of the atomic_uint object
 * @o: old value expected
 * @v: new value to add
 *
 * return: true if the value is added
 */

static __always_inline bool atomic_uint_add_ne (atomic_uint * a, unsigned int o,
                                                unsigned int v)
    {
    return atomic_int_add_ne ((atomic_int *) a, (int) o, (int) v);
    }

/**
 * atomic_int_dec_nz - decrease a atomic_int variable if it is not zero
 * @a: address of the atomic_int object
 *
 * return: true if the value is added
 */

static __always_inline bool atomic_int_dec_nz (atomic_int * a)
    {
    return atomic_int_add_ne (a, 0, -1);
    }

/**
 * atomic_uint_dec_nz - decrease a atomic_uint variable if it is not zero
 * @a: address of the atomic_uint object
 *
 * return: true if the value is added
 */

static __always_inline bool atomic_uint_dec_nz (atomic_uint * a)
    {
    return atomic_int_add_ne ((atomic_int *) a, 0, (int) -1);
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCHM_ATOMIC_H__ */

