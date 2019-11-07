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

/*
high level description
----------------------

This stack trace library is used to get the link registers of the fault routine,
and the link registers of its parents. And the parents' link address are saved
in either in the current lr or the stack. If we can carefully simulate the lr
and stack operation, we can get all of them back.
The basic idea is, the routine itself will return to its parent. So we just parse
the instructions from the faulting PC, save the context (sp, lr, pc) in <trctx>,
<trctx> is defined as:

    struct trace_ctx
        {
        uint32_t * sp;
        uint16_t * lr;
        uint16_t * pc;
        };

and simulate the following special instructions:

    *) add    sp, sp, #imm

       when met this instruction, just update the trctx->sp as:

           trctx->sp += __count_regs (#imm);

    *) bcc    label

       when met this instruction, we always jump to higher address, means, if
       label is lower than the address of current instruction, we do nothing,
       and skip this instruction, if label is higher than current instruction,
       we branch to label and update the context like this:

           trctx->pc = label;

       of cause, the label should be carefully calculated

    *) bx     lr

       when met this instruction, means current routine is justing returning,
       it is impossible to use <bx lr> for function pointer invoking, so we just
       update the context as:

           trctx->pc = trctx->lr;

       and just tell the trace engine we found the link address

    *) cb{nz} label

       when met this instruction, just update the trctx->pc as:

           trctx->pc = label;

       we always branch to label, because in this instruction, label always higher
       than the address of current instruction, just like it is in <bcc label>
       label should be carefully calculated

    *) pop    {...}

       when met this instruction, there are two situations:
       1) PC in the register list
           in this situation, we just got the link address in the register list,
           we should adjust <trctx->sp> and get PC from the stack
       2) PC do not in the register list
           in this situation, we just adjust <trctx->sp> to track the stack pointer

    *) push   {...}

       when met this instruction, there are two situations:
       1) lr in the register list
           in this situation, we just got the link address in the lr just like
           <bx     lr>
       2) lr do not in the register list
           in this situation, we just adjust <trctx->sp> to track the stack pointer

    *) sub    sp, sp, #imm

       similar to <add    sp, sp, #imm>

    *) tbb    [pc, rm]

       when met this instruction, just update the trctx->pc as:

           trctx->pc += trctx->pc [0];

       as every entry of the table will lead us return to the parent

    *) tbh    [pc, rm]

       same to <tbb    [pc, rm]>
*/


#include <stdint.h>

#include <config.h>

#include <task.h>
#include <memtry.h>
#include <kprintf.h>
#include <irq.h>

#include <arch/exc.h>
#include <arch/trace.h>

/* macros */

#define NR_MAX_LOOPS        (128)

/* typedefs */

struct trace_ctx
    {
    uint32_t * sp;
    uint16_t * lr;
    uint16_t * pc;
    };

/* inlines */

/*
 * __bits (x, h, l), return the bit value of x [h:l], if h == l, return the bit
 * of x [h] or x [l]
 */

static __noinline uint32_t __bits (uint32_t x, uint32_t h, uint32_t l)
    {
    return (x >> l) & ((1u << (h - l + 1)) - 1);
    }

/*
 * __is_t1, check if an instruction is 16-bit instruction
 */

static __always_inline bool __is_t1 (uint16_t instr)
    {
    return (instr >> 11) < 0x1d;
    }

/*
 * __count_regs, counting the number of registers in a register list
 */

static int __count_regs (uint32_t reglist)
    {
    int         regs = 0;
    static char ones_map [] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

    regs += ones_map [(reglist >>  0) & 0xf];
    regs += ones_map [(reglist >>  4) & 0xf];
#ifdef CONFIG_AARCH_M_MAINLINE
    regs += ones_map [(reglist >>  8) & 0xf];
    regs += ones_map [(reglist >> 12) & 0xf];
#endif

    return regs;
    }

#ifdef CONFIG_AARCH_M_MAINLINE

/* refer to ThumbExpandImm in the manual */

static uint32_t __thumb_expand_imm (uint32_t i, uint32_t imm3, uint32_t imm8)
    {
    uint32_t imm12 = (i << 11) | (imm3 << 8) | imm8;
    uint32_t m;

    switch (__bits (imm12, 11, 8))
        {
        case 0:
            return (imm8);
        case 1:
            return (imm8 << 16) | (imm8);
        case 2:
            return (imm8 << 24) | (imm8 << 8);
        case 3:
            return (imm8 << 24) | (imm8 << 16) | (imm8 << 8) | (imm8);
        default:
            m = __bits (imm12, 11, 7);
            return (imm8 << (32 - m)) | (imm8 >> m);
        }
    }

#endif

/*
 * __parse_t1
 *
 * return: false: continue parse next instruction
 *         true:  with pc == NULL, stop trace
 *                with pc != NULL, found return address
 *
 * NOTE: PC in arm is always the address of current instruction + 4, and in this
 *       function, trctx->pc is the address of current instruction + 2, so all
 *       PC relative value should be trctx->pc + 1 (trctx->pc has the type of u16)
 */

static bool __parse_t1 (uint16_t instr, struct trace_ctx * trctx)
    {

    /*
     * add<c> (sp with immediate) T2 (add<c> sp, sp, #<imm7>)
     * sub<c> (sp with immediate) T1 (sub<c> sp, sp, #<imm7>)
     *
     * bit [15: 8] = [1 0 1 1 0 0 0 0] (0xb0)
     * bit [7]     = op (0 - add, 1 - sub)
     * bit [ 6: 0] = imm7
     */

    if (__bits (instr, 15, 8) == 0xb0)
        {
        if (__bits (instr, 7, 7) == 0)
            {
            trctx->sp += __bits (instr, 6, 0);
            }
        else
            {
            trctx->sp -= __bits (instr, 6, 0);
            }

        return false;
        }

    /*
     * b<c> T1
     *
     * bit [15:12] = [1 1 0 1] (0xd)
     * bit [11: 8] = cond
     * bit [ 7: 0] = signed imm
     *
     * if the sign bit of the signed-imm8 is set, do nothing
     */

    if (__bits (instr, 15, 12) == 0xd)
        {
        if (__bits (instr, 11, 9) == 7)
            {

            /* UDF (cond = 0b1110) or SVC (cond = 0b1111) */

            return false;
            }

        if (__bits (instr, 7, 7) == 0)
            {
            trctx->pc += __bits (instr, 6, 0) + 1;
            }

        return false;
        }

    /*
     * b<c> T2
     *
     * bit [15:11] = [1  1  1  0  0] (0x1c)
     * bit [10: 0] = signed imm
     *
     * if the sign bit of the signed-imm11 is set, do nothing
     */

    if (__bits (instr, 15, 11) == 0x1c)
        {
        trctx->pc += __bits (instr, 9, 0) + 1;

        if (__bits (instr, 10, 10) != 0)        /* bit [10] is the sign bit */
            {

            /*
             * uncondition jump, if sign bit is set, just add
             *
             *     0b1111 1111 1111 1111 1111 1100 0000 0000
             *                                 \
             *                                  `-- bit 10
             *
             */

            trctx->pc += 0xfffffc00;
            }

        return false;
        }

    /*
     * bx<c> T1
     *
     * bit [15: 7] = [0 1 0 0 0 1 1 1 0] (0x8e)
     * bit [ 6: 3] = rm
     * bit [ 2: 0] = [0 0 0]
     *
     * if rm is lr, means return, as it is impossible to use <bx lr> for function
     * pointer invoke, by using <bx lr> for invoking function pointer, there is
     * no way to return, so it must be return
     */

    if ((__bits (instr, 15, 7) == 0x8e) && (__bits (instr, 2, 0) == 0))
        {
        if (__bits (instr, 6, 3) == 14)         /* check if rm is r14 (lr) */
            {
            trctx->pc = trctx->lr;
            return true;
            }

        return false;
        }

    /*
     * cb{n}z<c> T1
     *
     * bit [15:12] = [1 0 1 1] (0xb)
     * bit [11]    = op (1: cbnz, 0: cbz)
     * bit [10]    = [0]
     * bit [ 9]    = i
     * bit [ 8]    = [1]
     * bit [ 7: 3] = imm5
     * bit [ 2: 0] = rn
     *
     * offset = i:imm5
     */

    if ((__bits (instr, 15, 12) == 0xb) &&
        (__bits (instr, 10, 10) == 0x0) &&
        (__bits (instr,  8,  8) == 0x1))
        {
        trctx->pc += (__bits (instr, 9, 9) << 5) + __bits (instr, 7, 3) + 1;
        return false;
        }

    /*
     * mov<c> T1
     *
     * bit [15: 8] = [0 1 0 0 0 1 1 0] (0x46)
     * bit [ 7]    = D
     * bit [ 6: 3] = rm
     * bit [ 2: 0] = rd
     *
     * d = D:rd, we only handle when d == 13, as we have no way to get the stack
     * we just stop the trace
     */

    if ((__bits (instr, 15, 8) == 0x46) &&
        (__bits (instr,  7, 7) == 1)    &&
        (__bits (instr,  2, 0) == 5))
        {
        trctx->pc = NULL;
        return true;
        }

    /*
     * pop<c> T1
     *
     * bit [15: 9] = [1 0 1 1 1 1 0] (0x5e)
     * bit [ 8]    = P
     * bit [ 7: 0] = register list
     *
     * P: stand for PC
     */

    if (__bits (instr, 15, 9) == 0x5e)
        {
        trctx->sp += __count_regs (__bits (instr, 7, 0));

        if (__bits (instr, 8, 8) == 0)
            {
            return false;
            }

        trctx->pc = NULL;

        (void) mem_try (&trctx->pc, trctx->sp++, 2);

        return true;
        }

    /*
     * push<c> T1
     *
     * bit [15: 9] = [1 0 1 1 0 1 0] (0x5a)
     * bit [ 8]    = M
     * bit [ 7: 0] = register list
     *
     * M: stand for LR
     */

    if (__bits (instr, 15, 9) == 0x5a)
        {
        if (__bits (instr, 8, 8) == 0)
            {
            trctx->sp -= __count_regs (__bits (instr, 7, 0));

            return false;
            }

        trctx->pc = trctx->lr;

        return true;
        }

    return false;
    }

/*
 * __parse_t2
 *
 * return: false: continue parse next instruction
 *         true:  with pc == NULL, stop trace
 *                with pc != NULL, found return address
 *
 * NOTE: PC in arm is always the address of current instruction + 4, and in this
 *       function, trctx->pc is the address of current instruction + 4, so pc
 *       in this routine needless adjust
 */

static bool __parse_t2 (uint32_t instr, struct trace_ctx * trctx)
    {
#ifdef CONFIG_AARCH_M_MAINLINE

    /*
     * add<c> (sp with immediate) T3 (add<c> sp, sp, #<const>)
     *
     * bit [31:27] = [1 1 1 1 0] (0x1e)
     * bit [26]    = i
     * bit [25:21] = [0 1 0 0 0] (0x08)
     * bit [20]    = S
     * bit [19:15] = [1 1 0 1 0] (0x1a)
     * bit [14:12] = imm3
     * bit [11: 8] = rd (we only handle rd = sp)
     * bit [ 7: 0] = imm8
     */

    if ((__bits (instr, 31, 27) == 0x1e) &&
        (__bits (instr, 25, 21) == 0x08) &&
        (__bits (instr, 19, 15) == 0x1a))
        {
        if (__bits (instr, 11, 8) == 13)
            {
            uint32_t imm = __thumb_expand_imm (__bits (instr, 26, 26),
                                               __bits (instr, 14, 12),
                                               __bits (instr,  7,  0));

            /* stack is always aligned at 4 */

            trctx->sp += imm >> 2;
            }

        return false;
        }

    /*
     * add<c> (sp with immediate) T4 (add<c> sp, sp, #<imm12>)
     *
     * bit [31:27] = [1 1 1 1 0] (0x1e)
     * bit [26]    = i
     * bit [25:15] = [1 0 0 0 0 0 1 1 0 1 0] (0x41a)
     * bit [14:12] = imm3
     * bit [11: 8] = rd (we only handle rd = sp)
     * bit [ 7: 0] = imm8
     */

    if ((__bits (instr, 31, 27) == 0x1e) &&
        (__bits (instr, 25, 15) == 0x41a))
        {

        /* we only hand rd = sp (r13) */

        if (__bits (instr, 11, 8) == 13)
            {
            uint32_t imm = (__bits (instr, 26, 26) << 11) |
                           (__bits (instr, 14, 12) <<  8) |
                           (__bits (instr,  7,  0) <<  0);

            /* stack is always aligned at 4 */

            trctx->sp += imm >> 2;
            }

        return false;
        }

    /*
     * b<c> T3
     *
     * bit [31:27] = [1 1 1 1 0] (0x1e)
     * bit [26]    = sign
     * bit [25:22] = cond
     * bit [21:16] = imm6
     * bit [15:14] = [1 0] (0x2)
     * bit [13]    = J1
     * bit [12]    = [0]
     * bit [11]    = J2
     * bit [10: 0] = imm11
     *
     * offset = SignExtend (S:J2:J1:imm6:imm11:'0', 32);
     *
     * if the sign bit is set, do nothing
     */

    if ((__bits (instr, 31, 27) == 0x1e) &&
        (__bits (instr, 15, 14) == 0x02) &&
        (__bits (instr, 12, 12) == 0x0))
        {

        /* if cond <3:1> == '111' then SEE "Related encodings"; (section A5.3.4) */

        if (__bits (instr, 25, 23) == 7)
            {
            return false;
            }

        if (__bits (instr, 26, 26) == 0)
            {
            trctx->pc += (__bits (instr, 11, 11) << 18) |
                         (__bits (instr, 13, 13) << 17) |
                         (__bits (instr, 21, 16) << 11) |
                         (__bits (instr, 10,  0) <<  0);
            }

        return false;
        }

    /*
     * b<c> T4
     *
     * bit [31:27] = [1 1 1 1 0] (0x1e)
     * bit [26]    = sign
     * bit [25:16] = imm10
     * bit [15:14] = [1 0] (0x2)
     * bit [13]    = J1
     * bit [12]    = [1]
     * bit [11]    = J2
     * bit [10: 0] = imm11
     *
     * I1 = NOT (J1 EOR S);
     * I2 = NOT (J2 EOR S);
     *
     * offset = SignExtend (S:I1:I2:imm10:imm11:'0', 32);
     *
     * if the sign bit is set, do nothing
     */

    if ((__bits (instr, 31, 27) == 0x1e) &&
        (__bits (instr, 15, 14) == 0x02) &&
        (__bits (instr, 12, 12) == 0x1))
        {
        if (__bits (instr, 26, 26) == 0)
            {
            trctx->pc += (__bits (instr, 25, 16) << 11) | __bits (instr, 10, 0);

            /*
             * I1 = NOT (J1 EOR S);
             * I2 = NOT (J2 EOR S);
             * imm32 = SignExtend (S:I1:I2:imm10:imm11:'0', 32);
             *
             * and now, S = 0
             *
             * so, I = NOT (J EOR 0) = NOT (J)
             */

            if (__bits (instr, 13, 13) == 0)
                {
                trctx->pc += 0x00400000;
                }

            if (__bits (instr, 11, 11) == 0)
                {
                trctx->pc += 0x00200000;
                }
            }

        return false;
        }

    /*
     * pop<c> T2
     *
     * bit [31:16] = [1 1 1 0 1 0 0 0 1 0 1 1 1 1 0 1] (0xe8bd)
     * bit [15]    = PC
     * bit [14]    = LR
     * bit [12: 0] = register list
     */

    if (__bits (instr, 31, 16) == 0xe8bd)
        {
        trctx->sp += __count_regs (__bits (instr, 12, 0));

        if (__bits (instr, 14, 14) != 0)
            {
            if (mem_try (&trctx->lr, trctx->sp++, 2) != 0)
                {

                /* invalid stack, stop the trace */

                trctx->pc = NULL;
                return true;
                }
            }

        if (__bits (instr, 15, 15) != 0)
            {
            trctx->pc = NULL;
            (void) mem_try (&trctx->pc, trctx->sp++, 2);

            return true;
            }

        return false;
        }

    /*
     * pop<c> T3
     *
     * bit [31:16] = [1 1 1 1 1 0 0 0 0 1 0 1 1 1 0 1] (0xf85d)
     * bit [15:12] = rt
     * bit [11: 0] = [1 0 1 1 0 0 0 0 0 1 0 0] (0xb04)
     */

    if ((__bits (instr, 31, 16) == 0xf85d) &&
        (__bits (instr, 11,  0) == 0xb04))
        {
        switch (__bits (instr, 15, 12))
            {
            case 14:
                if (mem_try (&trctx->lr, trctx->sp++, 2) != 0)
                    {

                    /* invalid stack, stop the trace */

                    trctx->pc = NULL;
                    return true;
                    }
                break;
            case 15:
                trctx->pc = NULL;
                (void) mem_try (&trctx->pc, trctx->sp++, 2);
                return true;
            default:
                trctx->sp++;
                break;
            }

        return false;
        }

    /*
     * push<c> T2
     *
     * bit [31:16] = [1 1 1 0 1 0 0 1 0 0 1 0 1 1 0 1] (0xe92d)
     * bit [15]    = [0]
     * bit [14]    = LR
     * bit [13]    = [0]
     * bit [12: 0] = register list
     */

    if (__bits (instr, 31, 16) == 0xe92d)
        {
        if (__bits (instr, 14, 14) != 0)
            {
            trctx->pc = trctx->lr;
            return true;
            }

        trctx->sp -= __count_regs (__bits (instr, 12, 0));

        return false;
        }

    /*
     * push<c> T3
     *
     * bit [31:16] = [1 1 1 1 1 0 0 0 0 1 0 0 1 1 0 1] (0xf84d)
     * bit [15:12] = rt
     * bit [11: 0] = [1 1 0 1 0 0 0 0 0 1 0 0] (0xd04)
     */

    if ((__bits (instr, 31, 16) == 0xf84d) &&
        (__bits (instr, 11,  0) == 0xd04))
        {
        if (__bits (instr, 15, 12) == 14)
            {
            trctx->pc = trctx->lr;
            return true;
            }

        trctx->sp--;

        return false;
        }

    /*
     * sub<c> (sp with immediate) T2 (sub<c> sp, sp, #<const>)
     *
     * bit [31:27] = [1 1 1 1 0] (0x1e)
     * bit [26]    = i
     * bit [25:21] = [0 1 1 0 1] (0x0d)
     * bit [20]    = S
     * bit [19:15] = [1 1 0 1 0] (0x1a)
     * bit [14:12] = imm3
     * bit [11: 8] = rd (we only handle rd = sp)
     * bit [ 7: 0] = imm8
     */

    if ((__bits (instr, 31, 27) == 0x1e) &&
        (__bits (instr, 25, 21) == 0x0d) &&
        (__bits (instr, 19, 15) == 0x1a))
        {
        if (__bits (instr, 11, 8) == 13)
            {
            uint32_t imm = __thumb_expand_imm (__bits (instr, 26, 26),
                                               __bits (instr, 14, 12),
                                               __bits (instr, 7, 0));

            /* stack is always aligned at 4 */

            trctx->sp -= imm >> 2;
            }

        return false;
        }

    /*
     * sub<c> (sp with immediate) T3 (sub<c> sp, sp, #<imm12>)
     *
     * bit [31:27] = [1 1 1 1 0] (0x1e)
     * bit [26]    = i
     * bit [25:15] = [1 0 1 0 1 0 1 1 0 1 0] (0x55a)
     * bit [14:12] = imm3
     * bit [11: 8] = rd (we only handle rd = sp)
     * bit [ 7: 0] = imm8
     */

    if ((__bits (instr, 31, 27) == 0x1e) &&
        (__bits (instr, 25, 15) == 0x55a))
        {
        if (__bits (instr, 11, 8) == 13)
            {
            uint32_t imm = (__bits (instr, 26, 26) << 11) |
                           (__bits (instr, 14, 12) <<  8) |
                           (__bits (instr,  7,  0) <<  0);

            /* stack is always aligned at 4 */

            trctx->sp -= imm >> 2;
            }

        return false;
        }

    /*
     * tbb<c> [rn, rm]
     *
     * bit [31:20] = [1 1 1 0 1 0 0 0 1 1 0 1] (0xe8d)
     * bit [19:16] = rn
     * bit [15: 5] = [1 1 1 1 0 0 0 0 0 0 0] (0x780)
     * bit [4]     = H
     * bit [ 3: 0] = rm
     *
     * we can only handle rn == PC, and this should be the most cases. and we
     * always use the index 0, as every entry should return to the same address
     */

    if ((__bits (instr, 31, 20) == 0xe8d) &&
        (__bits (instr, 15,  5) == 0x780))
        {
        if (__bits (instr, 19, 16) != 15)
            {

            /* base is not PC, can not go on, stop the trace */

            trctx->pc = NULL;
            return true;
            }

        if (__bits (instr, 4, 4) == 0)
            {

            /* tbb */

            uint8_t  off;

            if (unlikely (mem_try (&off, trctx->pc, 0) != 0))
                {
                trctx->pc = NULL;
                }
            else
                {
                trctx->pc += off;
                }
            }
        else
            {

            /* tbh */

            uint16_t off;

            if (unlikely (mem_try (&off, trctx->pc, 1) != 0))
                {
                trctx->pc = NULL;
                }
            else
                {
                trctx->pc += off;
                }
            }

        return false;
        }
#endif

    return false;
    }

/*
 * find return address by parse, simulate, fake instructions
 *
 * return: 0 on success, negtive value on error
 */

static int __find_ra (struct trace_ctx * trctx)
    {
    uint16_t   instr_t1;
    uint32_t   instr_t2   = 0;
    uint16_t * loop       = NULL;   /* last loop code block begin address */
    uint16_t * last       = NULL;   /* last pc, used to check back branch */

    /*
     * there maybe following condition:
     *
     *                  low address
     *                |
     *                |
     *                |
     *                |<-------+
     *                |        |
     *       +------->|        |
     *       |        |        |
     *       |        |<-------+--+
     *       |        |        |  |
     *       |        |--------+  |
     *       |        |           |
     *       +--------|           |
     *                |           |
     *                |-----------+
     *                |
     *                |
     *                |
     *                  high address
     *
     * add loop_count and NR_MAX_LOOPS to prevent search the infinite loop
     */

    int        loop_count = 0;


    if (mem_try (&instr_t1, trctx->pc++, 1) != 0)
        {
        trctx->pc = trctx->lr;
        trctx->lr = NULL;

        return 0;
        }

    do
        {
        if (instr_t2 != 0)
            {
            instr_t2 |= instr_t1;

            if (__parse_t2 (instr_t2, trctx))
                {
                return trctx->pc == NULL ? -1 : 0;
                }

            instr_t2 = 0;
            }
        else
            {
            if (!__is_t1 (instr_t1))
                {
                instr_t2 = ((uint32_t) instr_t1) << 16;
                continue;
                }

            if (__parse_t1 (instr_t1, trctx))
                {
                return trctx->pc == NULL ? -1 : 0;
                }
            }

        /* if last instruction is a unconditional branch back */

        if ((uintptr_t) last > (uintptr_t) trctx->pc)
            {
            if (loop == trctx->pc)
                {
                return -1;
                }

            if (++loop_count >= NR_MAX_LOOPS)
                {
                return -1;
                }

            loop = trctx->pc;
            }

        last = trctx->pc;
        } while (mem_try (&instr_t1, trctx->pc++, 1) == 0);

    return -1;
    }

void __dump_stack (uint32_t * last_sp, uint32_t * sp)
    {
    int      count = 0;
    uint32_t content;
    uint32_t size = ((uint32_t) (sp - last_sp)) << 2;

    kprintf (" [%08x] =\n    {", size);

    while (last_sp != sp)
        {
        if ((count & 3) == 0)
            {
            kprintf ("\n    %08x: ", last_sp);
            }

        if (mem_try (&content, last_sp, 2) == 0)
            {
            kprintf ("%08x ", content);
            }
        else
            {
            kprintf ("???????? ");
            }

        count++;
        last_sp++;
        }

    kprintf ("\n    }\n");
    }

/*
 * back_trace - show the stack trace info for a context
 * sp: the stack pointer of the context
 * lr: the link register of the context
 * pc: the program counter of the context
 *
 * return: NA
 */

void back_trace (uint32_t sp, uint32_t lr, uint32_t pc)
    {
    struct trace_ctx trctx;
    bool             exit;
    uint32_t       * last_sp = NULL;

    trctx.sp  = (uint32_t *) sp;
    trctx.lr  = (uint16_t *) lr;
    trctx.pc  = (uint16_t *) pc;

    kprintf ("\n%scall stack%s\n",
             "===================================",
             "===================================");

    do
        {
        exit = ((uintptr_t) trctx.pc == (uintptr_t) task_exit);

        trctx.lr = (uint16_t *) (((uint32_t) trctx.lr) & ~1u);
        trctx.pc = (uint16_t *) (((uint32_t) trctx.pc) & ~1u);

        if (last_sp != NULL)
            {
            __dump_stack (last_sp, trctx.sp);
            }

        kprintf ("\nPC: %08x\nSP: %08x", trctx.pc, trctx.sp);

        last_sp = trctx.sp;
        } while (!exit && (__find_ra (&trctx) != -1));

    if (int_context () || current == NULL)
        {
        __dump_stack (last_sp, trctx.sp);
        }
    else
        {
        __dump_stack (last_sp, (uint32_t *) (current->stack_base + current->stack_size));
        }

    kprintf ("\n");
    }

/*
 * call_trace - show the stack trace info for the caller
 *
 * return: NA
 */

__naked void call_trace (void)
    {
    __asm__ __volatile__
        (
"       .syntax unified                                                      \n"
"                                                                            \n"
"       .extern back_trace                                                   \n"
"                                                                            \n"
"       mov     r0, sp                                                       \n"
"       movs    r1, #0      @ set lr = 0 and pc = lr to skip ourself         \n"
"       mov     r2, lr                                                       \n"
"       ldr     r3, =back_trace                                              \n"
"       bx      r3                                                           \n"
        );
    }

