/*----------------------------------------------------------------------------
 * Copyright (c) <2020>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/


#ifndef __TESTER_H__
#define __TESTER_H__

#include <stdint.h>
#include <compiler-test.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedefs */
typedef struct tester_statistics
    {
    uint32_t totals;
    uint32_t fails;
    } tester_statistics_t;

typedef struct tester_case
    {
    const char *name;
    uint32_t timeout;
    void (*entry) (struct tester_case *);
    void (*setup) (void);
    void (*teardown) (void);
    tester_statistics_t *stats;
    } tester_case_t;

/* macros */

#define TESTER_SECTION      tester

#define __TESTER_CASE_DEF(name, entry, setup, teardown, timeout, tag)           \
    static tester_statistics_t __CONCAT (stats, __CONCAT (entry, tag));         \
    const tester_case_t __CONCAT (entry, tag) __section__ (TESTER_SECTION) =    \
        {                                                                       \
        name,                                                                   \
        timeout,                                                                \
        entry,                                                                  \
        setup,                                                                  \
        teardown,                                                               \
        &__CONCAT (stats, __CONCAT (entry, tag))                                \
        }

#define TESTER_CASE_DEF(name, entry, setup, teardown, timeout) \
    __TESTER_CASE_DEF(name, entry, setup, teardown, timeout, __LINE__)

/* externs */

extern char __section_start__ (TESTER_SECTION) [];
extern char __section_end__   (TESTER_SECTION) [];


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CMDER_H__ */