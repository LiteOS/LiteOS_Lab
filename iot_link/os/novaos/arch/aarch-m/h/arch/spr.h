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

#ifndef __AARCH_M_SPR_H__
#define __AARCH_M_SPR_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedef */

struct scb
    {
    volatile uint32_t cpuid;
    volatile uint32_t icsr;
#ifdef CONFIG_HAS_VTOR
    volatile uint32_t vtor;
#else
    volatile uint32_t reserved0;
#endif
    volatile uint32_t aircr;
    volatile uint32_t scr;
    volatile uint32_t ccr;
#ifdef CONFIG_AARCH_M_BASELINE
    volatile uint32_t reserved1;
#else
    volatile uint32_t shpr1;
#endif
    volatile uint32_t shpr2;
    volatile uint32_t shpr3;
    volatile uint32_t shcsr;
#ifdef CONFIG_AARCH_M_MAINLINE
    volatile uint32_t cfsr;
    volatile uint32_t hfsr;
    volatile uint32_t dfsr;
    volatile uint32_t mmfar;
    volatile uint32_t bfar;
    volatile uint32_t afsr;
    volatile uint32_t id_pfr [2u];
    volatile uint32_t id_dfr;
    volatile uint32_t id_afr;
    volatile uint32_t id_mfr [4u];
    volatile uint32_t id_isar [5u];
    volatile uint32_t reserved2;
#ifdef CONFIG_CACHE
    volatile uint32_t clidr;
    volatile uint32_t ctr;
    volatile uint32_t ccsidr;
    volatile uint32_t csselr;
#else
    volatile uint32_t reserved3 [4];
#endif
    volatile uint32_t cpacr;
#endif
    };

/* macros */

/*
 * definations for scb->ccr
 */

#ifdef  CONFIG_HAS_CACHE
#define CCR_BP_SHIFT                    (18u)
#define CCR_BP                          (1u << CCR_BP_SHIFT)
#define CCR_BP_MASK                     (CCR_BP)

#define CCR_IC_SHIFT                    (17u)
#define CCR_IC                          (1u << CCR_IC_SHIFT)
#define CCR_IC_MASK                     (CCR_IC)

#define CCR_DC_SHIFT                    (16u)
#define CCR_DC                          (1u << CCR_DC_SHIFT)
#define CCR_DC_MASK                     (CCR_DC)
#endif

#define CCR_STKALIGN_SHIFT              (9u)
#define CCR_STKALIGN                    (1u << CCR_STKALIGN_SHIFT)
#define CCR_STKALIGN_MASK               (CCR_STKALIGN)

#ifdef  CONFIG_AARCH_M_MAINLINE
#define CCR_BFHFNMIGN_SHIFT             (8u)
#define CCR_BFHFNMIGN                   (1u << CCR_BFHFNMIGN_SHIFT)
#define CCR_BFHFNMIGN_MASK              (CCR_BFHFNMIGN)

#define CCR_DIV_0_TRP_SHIFT             (4u)
#define CCR_DIV_0_TRP                   (1u << CCR_DIV_0_TRP_SHIFT)
#define CCR_DIV_0_TRP_MASK              (CCR_DIV_0_TRP)
#endif

#define CCR_UNALIGN_TRP_SHIFT           (3u)
#define CCR_UNALIGN_TRP                 (1u << CCR_UNALIGN_TRP_SHIFT)
#define CCR_UNALIGN_TRP_MASK            (CCR_UNALIGN_TRP)

#ifdef  CONFIG_AARCH_M_MAINLINE
#define CCR_USERSETMPEND_SHIFT          (1u)
#define CCR_USERSETMPEND                (1u << CCR_USERSETMPEND_SHIFT)
#define CCR_USERSETMPEND_MASK           (CCR_USERSETMPEND)

#define CCR_NONBASETHRDENA_SHIFT        (0)
#define CCR_NONBASETHRDENA              (1u << CCR_NONBASETHRDENA_SHIFT)
#define CCR_NONBASETHRDENA_MASK         (CCR_NONBASETHRDENA)
#endif

/*
 * definations for scb->shcsr
 */

#ifdef  CONFIG_AARCH_M_MAINLINE
#define SHCSR_USGFAULTENA_SHIFT         (18u)
#define SHCSR_USGFAULTENA               (1u << SHCSR_USGFAULTENA_SHIFT)
#define SHCSR_USGFAULTENA_MASK          (SHCSR_USGFAULTENA)

#define SHCSR_BUSFAULTENA_SHIFT         (17u)
#define SHCSR_BUSFAULTENA               (1u << SHCSR_BUSFAULTENA_SHIFT)
#define SHCSR_BUSFAULTENA_MASK          (SHCSR_BUSFAULTENA)

#define SHCSR_MEMFAULTENA_SHIFT         (16u)
#define SHCSR_MEMFAULTENA               (1u << SHCSR_MEMFAULTENA_SHIFT)
#define SHCSR_MASKAULTENA_Msk           (SHCSR_MEMFAULTENA)
#endif

#define SHCSR_SVCALLPENDED_SHIFT        (15u)
#define SHCSR_SVCALLPENDED              (1u << SHCSR_SVCALLPENDED_SHIFT)
#define SHCSR_SVCALLPENDED_MASK         (SHCSR_SVCALLPENDED)

#ifdef  CONFIG_AARCH_M_MAINLINE
#define SHCSR_BUSFAULTPENDED_SHIFT      (14u)
#define SHCSR_BUSFAULTPENDED            (1u << SHCSR_BUSFAULTPENDED_SHIFT)
#define SHCSR_BUSFAULTPENDED_MASK       (SHCSR_BUSFAULTPENDED)

#define SHCSR_MEMFAULTPENDED_SHIFT      (13u)
#define SHCSR_MEMFAULTPENDED            (1u << SHCSR_MEMFAULTPENDED_SHIFT)
#define SHCSR_MASKAULTPENDED_Msk        (SHCSR_MEMFAULTPENDED)

#define SHCSR_USGFAULTPENDED_SHIFT      (12u)
#define SHCSR_USGFAULTPENDED            (1u << SHCSR_USGFAULTPENDED_SHIFT)
#define SHCSR_USGFAULTPENDED_MASK       (SHCSR_USGFAULTPENDED)

#define SHCSR_SYSTICKACT_SHIFT          (11u)
#define SHCSR_SYSTICKACT                (1u << SHCSR_SYSTICKACT_SHIFT)
#define SHCSR_SYSTICKACT_MASK           (SHCSR_SYSTICKACT)

#define SHCSR_PENDSVACT_SHIFT           (10u)
#define SHCSR_PENDSVACT                 (1u << SHCSR_PENDSVACT_SHIFT)
#define SHCSR_PENDSVACT_MASK            (SHCSR_PENDSVACT)

#define SHCSR_MONITORACT_SHIFT          (8u)
#define SHCSR_MONITORACT                (1u << SHCSR_MONITORACT_SHIFT)
#define SHCSR_MASKTORACT_Msk            (SHCSR_MONITORACT)

#define SHCSR_SVCALLACT_SHIFT           (7u)
#define SHCSR_SVCALLACT                 (1u << SHCSR_SVCALLACT_SHIFT)
#define SHCSR_SVCALLACT_MASK            (SHCSR_SVCALLACT)

#define SHCSR_USGFAULTACT_SHIFT         (3u)
#define SHCSR_USGFAULTACT               (1u << SHCSR_USGFAULTACT_SHIFT)
#define SHCSR_USGFAULTACT_MASK          (SHCSR_USGFAULTACT)

#define SHCSR_BUSFAULTACT_SHIFT         (1u)
#define SHCSR_BUSFAULTACT               (1u << SHCSR_BUSFAULTACT_SHIFT)
#define SHCSR_BUSFAULTACT_MASK          (SHCSR_BUSFAULTACT)

#define SHCSR_MEMFAULTACT_SHIFT         (0u)
#define SHCSR_MEMFAULTACT               (1u << SHCSR_MEMFAULTACT_SHIFT)
#define SHCSR_MASKAULTACT_Msk           (SHCSR_MEMFAULTACT)

/*
 * definations for scb->cfsr, cfsr has ufsr, bfsr and mmfsr
 */

/* cfsr.ufsr */

#define CFSR_MEMFAULTSR_SHIFT           (0u)
#define CFSR_MEMFAULTSR_MASK            (0xffu << CFSR_MEMFAULTSR_SHIFT)

#define CFSR_MMARVALID_SHIFT            (SHCSR_MEMFAULTACT_SHIFT + 7u)
#define CFSR_MMARVALID                  (1u << CFSR_MMARVALID_SHIFT)
#define CFSR_MMARVALID_MASK             (CFSR_MMARVALID)

#define CFSR_MLSPERR_SHIFT              (SHCSR_MEMFAULTACT_SHIFT + 5)
#define CFSR_MLSPERR                    (1u << CFSR_MLSPERR_SHIFT)
#define CFSR_MLSPERR_MASK               (CFSR_MLSPERR)

#define CFSR_MSTKERR_SHIFT              (SHCSR_MEMFAULTACT_SHIFT + 4)
#define CFSR_MSTKERR                    (1u << CFSR_MSTKERR_SHIFT)
#define CFSR_MSTKERR_MASK               (CFSR_MSTKERR)

#define CFSR_MUNSTKERR_SHIFT            (SHCSR_MEMFAULTACT_SHIFT + 3)
#define CFSR_MUNSTKERR                  (1u << CFSR_MUNSTKERR_SHIFT)
#define CFSR_MUNSTKERR_MASK             (CFSR_MUNSTKERR)

#define CFSR_DACCVIOL_SHIFT             (SHCSR_MEMFAULTACT_SHIFT + 1)
#define CFSR_DACCVIOL                   (1u << CFSR_DACCVIOL_SHIFT)
#define CFSR_DACCVIOL_MASK              (CFSR_DACCVIOL)

#define CFSR_IACCVIOL_SHIFT             (SHCSR_MEMFAULTACT_SHIFT + 0)
#define CFSR_IACCVIOL                   (1u << CFSR_IACCVIOL_SHIFT)
#define CFSR_IACCVIOL_MASK              (CFSR_IACCVIOL)

/* cfsr.bfsr */

#define CFSR_USGFAULTSR_SHIFT           (16u)
#define CFSR_USGFAULTSR_MASK            (0xffffu << CFSR_USGFAULTSR_SHIFT)

#define CFSR_DIVBYZERO_SHIFT            (CFSR_USGFAULTSR_SHIFT + 9u)
#define CFSR_DIVBYZERO                  (1u << CFSR_DIVBYZERO_SHIFT)
#define CFSR_DIVBYZERO_MASK             (CFSR_DIVBYZERO)

#define CFSR_UNALIGNED_SHIFT            (CFSR_USGFAULTSR_SHIFT + 8u)
#define CFSR_UNALIGNED                  (1u << CFSR_UNALIGNED_SHIFT)
#define CFSR_UNALIGNED_MASK             (CFSR_UNALIGNED)

#define CFSR_NOCP_SHIFT                 (CFSR_USGFAULTSR_SHIFT + 3)
#define CFSR_NOCP                       (1u << CFSR_NOCP_SHIFT)
#define CFSR_NOCP_MASK                  (CFSR_NOCP)

#define CFSR_INVPC_SHIFT                (CFSR_USGFAULTSR_SHIFT + 2)
#define CFSR_INVPC                      (1u << CFSR_INVPC_SHIFT)
#define CFSR_INVPC_MASK                 (CFSR_INVPC)

#define CFSR_INVSTATE_SHIFT             (CFSR_USGFAULTSR_SHIFT + 1)
#define CFSR_INVSTATE                   (1u << CFSR_INVSTATE_SHIFT)
#define CFSR_INVSTATE_MASK              (CFSR_INVSTATE)

#define CFSR_UNDEFINSTR_SHIFT           (CFSR_USGFAULTSR_SHIFT + 0)
#define CFSR_UNDEFINSTR                 (1u << CFSR_UNDEFINSTR_SHIFT)
#define CFSR_UNDEFINSTR_MASK            (CFSR_UNDEFINSTR)

/* cfsr.mmfsr */

#define CFSR_BUSFAULTSR_SHIFT           (8u)
#define CFSR_BUSFAULTSR_MASK            (0xffu << CFSR_BUSFAULTSR_SHIFT)

#define CFSR_BFARVALID_SHIFT            (CFSR_BUSFAULTSR_SHIFT + 7u)
#define CFSR_BFARVALID                  (1u << CFSR_BFARVALID_SHIFT)
#define CFSR_BFARVALID_MASK             (CFSR_BFARVALID)

#define CFSR_LSPERR_SHIFT               (CFSR_BUSFAULTSR_SHIFT + 5)
#define CFSR_LSPERR                     (1u << CFSR_LSPERR_SHIFT)
#define CFSR_LSPERR_MASK                (CFSR_LSPERR)

#define CFSR_STKERR_SHIFT               (CFSR_BUSFAULTSR_SHIFT + 4)
#define CFSR_STKERR                     (1u << CFSR_STKERR_SHIFT)
#define CFSR_STKERR_MASK                (CFSR_STKERR)

#define CFSR_UNSTKERR_SHIFT             (CFSR_BUSFAULTSR_SHIFT + 3)
#define CFSR_UNSTKERR                   (1u << CFSR_UNSTKERR_SHIFT)
#define CFSR_UNSTKERR_MASK              (CFSR_UNSTKERR)

#define CFSR_IMPRECISERR_SHIFT          (CFSR_BUSFAULTSR_SHIFT + 2)
#define CFSR_IMPRECISERR                (1u << CFSR_IMPRECISERR_SHIFT)
#define CFSR_IMPRECISERR_MASK           (CFSR_IMPRECISERR)

#define CFSR_PRECISERR_SHIFT            (CFSR_BUSFAULTSR_SHIFT + 1)
#define CFSR_PRECISERR                  (1u << CFSR_PRECISERR_SHIFT)
#define CFSR_PRECISERR_MASK             (CFSR_PRECISERR)

#define CFSR_IBUSERR_SHIFT              (CFSR_BUSFAULTSR_SHIFT + 0)
#define CFSR_IBUSERR                    (1u << CFSR_IBUSERR_SHIFT)
#define CFSR_IBUSERR_MASK               (CFSR_IBUSERR)
#endif

static struct scb * const scb = (struct scb *) 0xe000ed00;

/* inlines */

static __always_inline uint32_t __get_ipsr (void)
    {
    uint32_t ipsr;

    __asm__ __volatile__ ("mrs %0, ipsr\n\r" : "=r" (ipsr));

    return ipsr;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AARCH_M_SPR_H__ */

