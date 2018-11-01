/*!
    \file  gd32f3x0_wwdgt.h
    \brief definitions for the WWDGT 
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#ifndef GD32F3X0_WWDGT_H
#define GD32F3X0_WWDGT_H

#include "gd32f3x0.h"

/* WWDGT definitions */
#define WWDGT                       WWDGT_BASE

/* registers definitions */
#define WWDGT_CTL                   REG32((WWDGT) + 0x00U)                     /*!< WWDGT control register */
#define WWDGT_CFG                   REG32((WWDGT) + 0x04U)                     /*!< WWDGT configuration register */
#define WWDGT_STAT                  REG32((WWDGT) + 0x08U)                     /*!< WWDGT status register */

/* bits definitions */
/* WWDGT_CTL */
#define WWDGT_CTL_CNT               BITS(0,6)                                  /*!< WWDGT counter value */
#define WWDGT_CTL_WDGTEN            BIT(7)                                     /*!< WWDGT counter enable */

/* WWDGT_CFG */
#define WWDGT_CFG_WIN               BITS(0,6)                                  /*!< WWDGT counter window value */
#define WWDGT_CFG_PSC               BITS(7,8)                                  /*!< WWDGT prescaler divider value */
#define WWDGT_CFG_EWIE              BIT(9)                                     /*!< WWDGT early wakeup interrupt enable */

/* WWDGT_STAT */
#define WWDGT_STAT_EWIF             BIT(0)                                     /*!< WWDGT early wakeup interrupt flag */

/* constants definitions */
/* WWDGT_CTL register value */
#define CTL_CNT(regval)             (BITS(0,6) & ((uint32_t)(regval) << 0U))    /*!< write value to WWDGT_CTL_CNT bit field */

/* WWDGT_CFG register value */
#define CFG_WIN(regval)             (BITS(0,6) & ((uint32_t)(regval) << 0U))    /*!< write value to WWDGT_CFG_WIN bit field */

#define CFG_PSC(regval)             (BITS(7,8) & ((uint32_t)(regval) << 7U))    /*!< write value to WWDGT_CFG_PSC bit field */
#define WWDGT_CFG_PSC_DIV1          ((uint32_t)CFG_PSC(0))                     /*!< the time base of WWDGT = (PCLK1/4096)/1 */
#define WWDGT_CFG_PSC_DIV2          ((uint32_t)CFG_PSC(1))                     /*!< the time base of WWDGT = (PCLK1/4096)/2 */
#define WWDGT_CFG_PSC_DIV4          ((uint32_t)CFG_PSC(2))                     /*!< the time base of WWDGT = (PCLK1/4096)/4 */
#define WWDGT_CFG_PSC_DIV8          ((uint32_t)CFG_PSC(3))                     /*!< the time base of WWDGT = (PCLK1/4096)/8 */

/* function declarations */
/* reset the window watchdog timer configuration */
void wwdgt_deinit(void);
/* start the window watchdog timer counter */
void wwdgt_enable(void);

/* configure the window watchdog timer counter value */
void wwdgt_counter_update(uint16_t counter_value);
/* configure counter value, window value, and prescaler divider value */
void wwdgt_config(uint16_t counter, uint16_t window, uint32_t prescaler);

/* enable early wakeup interrupt of WWDGT */
void wwdgt_interrupt_enable(void);
/* check early wakeup interrupt state of WWDGT */
FlagStatus wwdgt_flag_get(void);
/* clear early wakeup interrupt state of WWDGT */
void wwdgt_flag_clear(void);

#endif /* GD32F3X0_WWDGT_H */
