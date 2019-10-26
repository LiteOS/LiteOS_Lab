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

#ifndef __CMDER_H__
#define __CMDER_H__

#include <stdint.h>

#include <common.h>
#include <ring.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define CMDER_BUFFER_SIZE       128
#define CMDER_NR_ARGUMENTS      8               /* including the command name */

#define CMDER_SECTION           cmder

#define __CMDER_CMD_DEF(name, desc, cmd, tag)                                   \
    const cmder_cmd_t __CONCAT (cmd, tag) __section__ (CMDER_SECTION) =         \
        {                                                                       \
        name,                                                                   \
        desc,                                                                   \
        cmd                                                                     \
        }

#define CMDER_CMD_DEF(name, desc, cmd)                                          \
    __CMDER_CMD_DEF (name, desc, cmd, __LINE__)

/* typedefs */

typedef struct cmder
    {
    char             input_buff [CMDER_BUFFER_SIZE];
    uint8_t          input_idx;
    uint8_t          compl_idx;
    uint8_t          shift_idx;
    uint8_t          position;
    uint8_t          line_end;
    ring_t           his_cmd;
    ring_t           his_idx;
    unsigned char (* getc) (uintptr_t);
    int           (* putc) (uintptr_t, unsigned char);
    uintptr_t        arg;       /* argument for those routines */
    } cmder_t;

typedef struct cmder_cmd
    {
    const char     * name;
    const char     * desc;
    int           (* cmd) (cmder_t * cmder, int argc, char * argv []);
    } cmder_cmd_t;

enum cmder_vk
    {
    cmder_vk_nul = 0x00,
    cmder_vk_ctrla,
    cmder_vk_ctrlb,
    cmder_vk_ctrlc,
    cmder_vk_ctrld,
    cmder_vk_ctrle,
    cmder_vk_ctrlf,
    cmder_vk_ctrlg,
    cmder_vk_ctrlh,
    cmder_vk_ctrli,
    cmder_vk_ctrlj,
    cmder_vk_ctrlk,
    cmder_vk_ctrll,
    cmder_vk_ctrlm,
    cmder_vk_ctrln,
    cmder_vk_ctrlo,
    cmder_vk_ctrlp,
    cmder_vk_ctrlq,
    cmder_vk_ctrlr,
    cmder_vk_ctrls,
    cmder_vk_ctrlt,
    cmder_vk_ctrlu,
    cmder_vk_ctrlv,
    cmder_vk_ctrlw,
    cmder_vk_ctrlx,
    cmder_vk_ctrly,
    cmder_vk_ctrlz,

    cmder_vk_esc,
    cmder_vk_fs,
    cmder_vk_gs,
    cmder_vk_rs,
    cmder_vk_us,

    /* printable, 0x20~0x7e */

    cmder_vk_del = 0x7f,

    cmder_vk_home,
    cmder_vk_end,
    cmder_vk_up,
    cmder_vk_down,
    cmder_vk_left,
    cmder_vk_right,
    };

/* externs */

extern char __section_start__ (CMDER_SECTION) [];
extern char __section_end__   (CMDER_SECTION) [];

extern int cmder_loop         (uintptr_t arg);
extern int cmder_printf       (cmder_t * cmder, const char * format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CMDER_H__ */

