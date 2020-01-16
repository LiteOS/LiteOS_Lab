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

#ifndef __COMPILER_TEST_H__
#define __COMPILER_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef  __CC_ARM
#define SECTION_NAME_PREFIX
#define SECTION_SYM_PREFIX
#define SECTION_SYM_START               $$Base
#define SECTION_SYM_END                 $$Limit
#else
#define SECTION_NAME_PREFIX
#define SECTION_SYM_PREFIX              __
#define SECTION_SYM_START               _start
#define SECTION_SYM_END                 _end
#endif

/*
 * __CONCAT - make a new symbol by join two sub-strings
 * @s1: string 1
 * @s2: string 2
 */

#ifdef  __CONCAT
#undef  __CONCAT
#endif

#define __CONCAT_RAW(s1, s2)    s1 ## s2
#define __CONCAT(s1, s2)        __CONCAT_RAW (s1, s2)

/*
 * __CVTSTR - create a string as string -> "string"
 * @s: the input string
 */

#define __CVTSTR_RAW(s)         #s
#define __CVTSTR(s)             __CVTSTR_RAW (s)

/*
 * __section__ - place a symbol in a specific section
 * @name: the section name.
 */
#ifndef __section__
#define __section__(name)               \
    __attribute__ ((section (SECTION_NAME_PREFIX __CVTSTR (name))))
#endif

/*
 * __section_start__ - import to the start of a section
 * @name: the section name.
 */
#ifndef __section_start__
#define __section_start__(name)         \
    __CONCAT (__CONCAT (SECTION_SYM_PREFIX, name), SECTION_SYM_START)
#endif

/*
 * __section_end__ - import to the end of a section
 * @name: the section name.
 */
#ifndef __section_end__
#define __section_end__(name)           \
    __CONCAT (__CONCAT (SECTION_SYM_PREFIX, name), SECTION_SYM_END)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMPILER_TEST_H__ */