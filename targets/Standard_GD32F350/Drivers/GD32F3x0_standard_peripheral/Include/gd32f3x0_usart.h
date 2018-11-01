/*!
    \file  gd32f3x0_usart.h
    \brief definitions for the USART
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-5-31, V1.0.0, firmware for GD32F3x0
*/

#ifndef GD32F3X0_USART_H
#define GD32F3X0_USART_H

#include "gd32f3x0.h"

/* USARTx(x=0,1) definitions */
#define USART0                        (USART_BASE + 0x0000F400U)
#define USART1                        USART_BASE

/* registers definitions */
#define USART_CTL0(usartx)            REG32((usartx) + 0x00U)        /*!< USART control register 0 */
#define USART_CTL1(usartx)            REG32((usartx) + 0x04U)        /*!< USART control register 1 */
#define USART_CTL2(usartx)            REG32((usartx) + 0x08U)        /*!< USART control register 2 */
#define USART_BAUD(usartx)            REG32((usartx) + 0x0CU)        /*!< USART baud rate register */
#define USART_GP(usartx)              REG32((usartx) + 0x10U)        /*!< USART guard time and prescaler register */
#define USART_RT(usartx)              REG32((usartx) + 0x14U)        /*!< USART receiver timeout register */
#define USART_CMD(usartx)             REG32((usartx) + 0x18U)        /*!< USART command register */
#define USART_STAT(usartx)            REG32((usartx) + 0x1CU)        /*!< USART status register */
#define USART_INTC(usartx)            REG32((usartx) + 0x20U)        /*!< USART status clear register */
#define USART_RDATA(usartx)           REG32((usartx) + 0x24U)        /*!< USART receive data register */
#define USART_TDATA(usartx)           REG32((usartx) + 0x28U)        /*!< USART transmit data register */
#define USART_RFCS(usartx)            REG32((usartx) + 0xD0U)        /*!< USART receive FIFO control and status register */

/* bits definitions */
/* USARTx_CTL0 */
#define USART_CTL0_UEN                BIT(0)                         /*!< USART enable */
#define USART_CTL0_UESM               BIT(1)                         /*!< USART enable in deep-sleep mode */
#define USART_CTL0_REN                BIT(2)                         /*!< receiver enable */
#define USART_CTL0_TEN                BIT(3)                         /*!< transmitter enable */
#define USART_CTL0_IDLEIE             BIT(4)                         /*!< idle line detected interrupt enable */
#define USART_CTL0_RBNEIE             BIT(5)                         /*!< read data buffer not empty interrupt and overrun error interrupt enable */
#define USART_CTL0_TCIE               BIT(6)                         /*!< transmission complete interrupt enable */
#define USART_CTL0_TBEIE              BIT(7)                         /*!< transmitter register empty interrupt enable */
#define USART_CTL0_PERRIE             BIT(8)                         /*!< parity error interrupt enable */
#define USART_CTL0_PM                 BIT(9)                         /*!< parity mode */
#define USART_CTL0_PCEN               BIT(10)                        /*!< parity control enable */
#define USART_CTL0_WM                 BIT(11)                        /*!< wakeup method in mute mode */
#define USART_CTL0_WL                 BIT(12)                        /*!< word length */
#define USART_CTL0_MEN                BIT(13)                        /*!< mute mode enable */
#define USART_CTL0_AMIE               BIT(14)                        /*!< address match interrupt enable */
#define USART_CTL0_OVSMOD             BIT(15)                        /*!< oversample mode */
#define USART_CTL0_DED                BITS(16,20)                    /*!< driver enable deassertion time */
#define USART_CTL0_DEA                BITS(21,25)                    /*!< driver enable assertion time */
#define USART_CTL0_RTIE               BIT(26)                        /*!< receiver timeout interrupt enable */
#define USART_CTL0_EBIE               BIT(27)                        /*!< end of block interrupt enable */

/* USARTx_CTL1 */
#define USART_CTL1_ADDM               BIT(4)                         /*!< address detection mode */
#define USART_CTL1_LBLEN              BIT(5)                         /*!< LIN break frame length */
#define USART_CTL1_LBDIE              BIT(6)                         /*!< LIN break detection interrupt enable */
#define USART_CTL1_CLEN               BIT(8)                         /*!< last bit clock pulse */
#define USART_CTL1_CPH                BIT(9)                         /*!< clock phase */
#define USART_CTL1_CPL                BIT(10)                        /*!< clock polarity */
#define USART_CTL1_CKEN               BIT(11)                        /*!< ck pin enable */
#define USART_CTL1_STB                BITS(12,13)                    /*!< stop bits length */
#define USART_CTL1_LMEN               BIT(14)                        /*!< LIN mode enable */
#define USART_CTL1_STRP               BIT(15)                        /*!< swap TX/RX pins */
#define USART_CTL1_RINV               BIT(16)                        /*!< RX pin level inversion */
#define USART_CTL1_TINV               BIT(17)                        /*!< TX pin level inversion */
#define USART_CTL1_DINV               BIT(18)                        /*!< data bit level inversion */
#define USART_CTL1_MSBF               BIT(19)                        /*!< most significant bit first */
#define USART_CTL1_ABDEN              BIT(20)                        /*!< auto baud rate enable */
#define USART_CTL1_ABDM               BITS(21,22)                    /*!< auto baud rate mode */
#define USART_CTL1_RTEN               BIT(23)                        /*!< receiver timeout enable */
#define USART_CTL1_ADDR               BITS(24,31)                    /*!< address of the USART terminal */

/* USARTx_CTL2 */
#define USART_CTL2_ERRIE              BIT(0)                         /*!< error interrupt enable in multibuffer communication */
#define USART_CTL2_IREN               BIT(1)                         /*!< IrDA mode enable */
#define USART_CTL2_IRLP               BIT(2)                         /*!< IrDA low-power */
#define USART_CTL2_HDEN               BIT(3)                         /*!< half-duplex enable */
#define USART_CTL2_NKEN               BIT(4)                         /*!< NACK enable in smartcard mode */
#define USART_CTL2_SCEN               BIT(5)                         /*!< smartcard mode enable */
#define USART_CTL2_DENR               BIT(6)                         /*!< DMA enable for reception */
#define USART_CTL2_DENT               BIT(7)                         /*!< DMA enable for transmission */
#define USART_CTL2_RTSEN              BIT(8)                         /*!< RTS enable */
#define USART_CTL2_CTSEN              BIT(9)                         /*!< CTS enable */
#define USART_CTL2_CTSIE              BIT(10)                        /*!< CTS interrupt enable */
#define USART_CTL2_OSB                BIT(11)                        /*!< one sample bit mode */
#define USART_CTL2_OVRD               BIT(12)                        /*!< overrun disable */
#define USART_CTL2_DDRE               BIT(13)                        /*!< disable DMA on reception error */
#define USART_CTL2_DEM                BIT(14)                        /*!< driver enable mode */
#define USART_CTL2_DEP                BIT(15)                        /*!< driver enable polarity mode */
#define USART_CTL2_SCRTNUM            BITS(17,19)                    /*!< smartcard auto-retry number */
#define USART_CTL2_WUM                BITS(20,21)                    /*!< wakeup mode from deep-sleep mode */
#define USART_CTL2_WUIE               BIT(22)                        /*!< wakeup from deep-sleep mode interrupt enable */

/* USARTx_BAUD */
#define USART_BAUD_FRADIV             BITS(0,3)                      /*!< fraction of baud-rate divider */
#define USART_BAUD_INTDIV             BITS(4,15)                     /*!< integer of baud-rate divider */

/* USARTx_GP */
#define USART_GP_PSC                  BITS(0,7)                      /*!< prescaler value for dividing the system clock */
#define USART_GP_GUAT                 BITS(8,15)                     /*!< guard time value in smartcard mode */

/* USARTx_RT */
#define USART_RT_RT                   BITS(0,23)                     /*!< receiver timeout threshold */
#define USART_RT_BL                   BITS(24,31)                    /*!< block length */

/* USARTx_CMD */
#define USART_CMD_ABDCMD              BIT(0)                         /*!< auto baudrate detection command */
#define USART_CMD_SBKCMD              BIT(1)                         /*!< send break command */
#define USART_CMD_MMCMD               BIT(2)                         /*!< mute mode command */
#define USART_CMD_RXFCMD              BIT(3)                         /*!< receive data flush command */
#define USART_CMD_TXFCMD              BIT(4)                         /*!< transmit data flush request */

/* USARTx_STAT */
#define USART_STAT_PERR               BIT(0)                         /*!< parity error flag */
#define USART_STAT_FERR               BIT(1)                         /*!< frame error flag */
#define USART_STAT_NERR               BIT(2)                         /*!< noise error flag */
#define USART_STAT_ORERR              BIT(3)                         /*!< overrun error */
#define USART_STAT_IDLEF              BIT(4)                         /*!< idle line detected flag */
#define USART_STAT_RBNE               BIT(5)                         /*!< read data buffer not empty */
#define USART_STAT_TC                 BIT(6)                         /*!< transmission completed */
#define USART_STAT_TBE                BIT(7)                         /*!< transmit data register empty */
#define USART_STAT_LBDF               BIT(8)                         /*!< LIN break detected flag */
#define USART_STAT_CTSF               BIT(9)                         /*!< CTS change flag */
#define USART_STAT_CTS                BIT(10)                        /*!< CTS level */
#define USART_STAT_RTF                BIT(11)                        /*!< receiver timeout flag */
#define USART_STAT_EBF                BIT(12)                        /*!< end of block flag */
#define USART_STAT_ABDE               BIT(14)                        /*!< auto baudrate detection error */
#define USART_STAT_ABDF               BIT(15)                        /*!< auto baudrate detection flag */
#define USART_STAT_BSY                BIT(16)                        /*!< busy flag */
#define USART_STAT_AMF                BIT(17)                        /*!< address match flag */
#define USART_STAT_SBF                BIT(18)                        /*!< send break flag */
#define USART_STAT_RWU                BIT(19)                        /*!< receiver wakeup from mute mode */
#define USART_STAT_WUF                BIT(20)                        /*!< wakeup from deep-sleep mode flag */
#define USART_STAT_TEA                BIT(21)                        /*!< transmit enable acknowledge flag */
#define USART_STAT_REA                BIT(22)                        /*!< receive enable acknowledge flag */

/* USARTx_INTC */
#define USART_INTC_PEC                BIT(0)                         /*!< parity error clear */
#define USART_INTC_FEC                BIT(1)                         /*!< frame error flag clear */
#define USART_INTC_NEC                BIT(2)                         /*!< noise detected clear */
#define USART_INTC_OREC               BIT(3)                         /*!< overrun error clear */
#define USART_INTC_IDLEC              BIT(4)                         /*!< idle line detected clear */
#define USART_INTC_TCC                BIT(6)                         /*!< transmission complete clear */
#define USART_INTC_LBDC               BIT(8)                         /*!< LIN break detected clear */
#define USART_INTC_CTSC               BIT(9)                         /*!< CTS change clear */
#define USART_INTC_RTC                BIT(11)                        /*!< receiver timeout clear */
#define USART_INTC_EBC                BIT(12)                        /*!< end of timeout clear */
#define USART_INTC_AMC                BIT(17)                        /*!< ADDR match clear */
#define USART_INTC_WUC                BIT(20)                        /*!< wakeup from deep-sleep mode clear */

/* USARTx_RDATA */
#define USART_RDATA_RDATA             BITS(0,8)                      /*!< receive data value */

/* USARTx_TDATA */
#define USART_TDATA_TDATA             BITS(0,8)                      /*!< transmit data value */

/* USARTx_RFCS */
#define USART_RFCS_ELNACK             BIT(0)                         /*!< early NACK */
#define USART_RFCS_RFEN               BIT(8)                         /*!< receive FIFO enable */
#define USART_RFCS_RFFIE              BIT(9)                         /*!< receive FIFO full interrupt enable */
#define USART_RFCS_RFE                BIT(10)                        /*!< receive FIFO empty flag */
#define USART_RFCS_RFF                BIT(11)                        /*!< receive FIFO full flag */
#define USART_RFCS_RFCNT              BITS(12,14)                    /*!< receive FIFO counter number */
#define USART_RFCS_RFFINT             BIT(15)                        /*!< receive FIFO full interrupt flag */

/* constants definitions */

/* define the USART bit position and its register index offset */
#define USART_REGIDX_BIT(regidx, bitpos)    (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos))
#define USART_REG_VAL(usartx, offset)       (REG32((usartx) + (((uint32_t)(offset) & 0xFFFFU) >> 6)))
#define USART_BIT_POS(val)                  ((uint32_t)(val) & 0x1FU)
#define USART_REGIDX_BIT2(regidx, bitpos, regidx2, bitpos2)   (((uint32_t)(regidx2) << 22) | (uint32_t)((bitpos2) << 16)\
                                                              | (((uint32_t)(regidx) << 6) | (uint32_t)(bitpos)))
#define USART_REG_VAL2(usartx, offset)       (REG32((usartx) + ((uint32_t)(offset) >> 22)))
#define USART_BIT_POS2(val)                  (((uint32_t)(val) & 0x1F0000U) >> 16)

/* register offset */
#define CTL0_REG_OFFSET              0x00U                           /*!< CTL0 register offset */
#define CTL1_REG_OFFSET              0x04U                           /*!< CTL1 register offset */
#define CTL2_REG_OFFSET              0x08U                           /*!< CTL2 register offset */
#define STAT_REG_OFFSET              0x1CU                           /*!< STAT register offset */
#define RFCS_REG_OFFSET              0xD0U                           /*!< RFCS register offset */

/* USART flags */
typedef enum{
    /* flags in STAT register */
    USART_FLAG_REA = USART_REGIDX_BIT(STAT_REG_OFFSET, 22U),         /*!< receive enable acknowledge flag */
    USART_FLAG_TEA = USART_REGIDX_BIT(STAT_REG_OFFSET, 21U),         /*!< transmit enable acknowledge flag */
    USART_FLAG_WUF = USART_REGIDX_BIT(STAT_REG_OFFSET, 20U),         /*!< wakeup from Deep-sleep mode flag */
    USART_FLAG_RWU = USART_REGIDX_BIT(STAT_REG_OFFSET, 19U),         /*!< receiver wakeup from mute mode */
    USART_FLAG_SBF = USART_REGIDX_BIT(STAT_REG_OFFSET, 18U),         /*!< send break flag */
    USART_FLAG_AMF = USART_REGIDX_BIT(STAT_REG_OFFSET, 17U),         /*!< ADDR match flag */
    USART_FLAG_BSY = USART_REGIDX_BIT(STAT_REG_OFFSET, 16U),         /*!< busy flag */
    USART_FLAG_ABDF = USART_REGIDX_BIT(STAT_REG_OFFSET, 15U),        /*!< auto baudrate detection flag */
    USART_FLAG_ABDE = USART_REGIDX_BIT(STAT_REG_OFFSET, 14U),        /*!< auto baudrate detection error */
    USART_FLAG_EBF = USART_REGIDX_BIT(STAT_REG_OFFSET, 12U),         /*!< end of block flag */
    USART_FLAG_RTF = USART_REGIDX_BIT(STAT_REG_OFFSET, 11U),         /*!< receiver timeout flag */
    USART_FLAG_CTS = USART_REGIDX_BIT(STAT_REG_OFFSET, 10U),         /*!< CTS level */
    USART_FLAG_CTSF = USART_REGIDX_BIT(STAT_REG_OFFSET, 9U),         /*!< CTS change flag */
    USART_FLAG_LBDF = USART_REGIDX_BIT(STAT_REG_OFFSET, 8U),         /*!< LIN break detected flag */
    USART_FLAG_TBE = USART_REGIDX_BIT(STAT_REG_OFFSET, 7U),          /*!< transmit data buffer empty */
    USART_FLAG_TC = USART_REGIDX_BIT(STAT_REG_OFFSET, 6U),           /*!< transmission complete */
    USART_FLAG_RBNE = USART_REGIDX_BIT(STAT_REG_OFFSET, 5U),         /*!< read data buffer not empty */
    USART_FLAG_IDLEF = USART_REGIDX_BIT(STAT_REG_OFFSET, 4U),        /*!< IDLE line detected flag */
    USART_FLAG_ORERR = USART_REGIDX_BIT(STAT_REG_OFFSET, 3U),        /*!< overrun error */
    USART_FLAG_NERR = USART_REGIDX_BIT(STAT_REG_OFFSET, 2U),         /*!< noise error flag */
    USART_FLAG_FERR = USART_REGIDX_BIT(STAT_REG_OFFSET, 1U),         /*!< frame error flag */
    USART_FLAG_PERR = USART_REGIDX_BIT(STAT_REG_OFFSET, 0U),         /*!< parity error flag */
    /* flags in RFCS register */
    USART_FLAG_RFFINT = USART_REGIDX_BIT(RFCS_REG_OFFSET, 15U),      /*!< receive FIFO full interrupt flag */
    USART_FLAG_RFF = USART_REGIDX_BIT(RFCS_REG_OFFSET, 11U),         /*!< receive FIFO full flag */
    USART_FLAG_RFE = USART_REGIDX_BIT(RFCS_REG_OFFSET, 10U),         /*!< receive FIFO empty flag */
}usart_flag_enum;

/* USART invert configure */
typedef enum {
    /* data bit level inversion */
    USART_DINV_ENABLE,                                               /*!< data bit level inversion */
    USART_DINV_DISABLE,                                              /*!< data bit level not inversion */
    /* TX pin level inversion */
    USART_TXPIN_ENABLE,                                              /*!< TX pin level inversion */               
    USART_TXPIN_DISABLE,                                             /*!< TX pin level not inversion */
    /* RX pin level inversion */
    USART_RXPIN_ENABLE,                                              /*!< RX pin level inversion */
    USART_RXPIN_DISABLE,                                             /*!< RX pin level not inversion */
    /* swap TX/RX pins */
    USART_SWAP_ENABLE,                                               /*!< swap TX/RX pins */                
    USART_SWAP_DISABLE,                                              /*!< not swap TX/RX pins */
}usart_invert_enum;

/* USART interrupt flags */
typedef enum
{
    /* interrupt flags in CTL0 register */
    USART_INT_EBIE_EBF = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 27U, STAT_REG_OFFSET, 12U),      /*!< end of block interrupt and flag */
    USART_INT_RTIE_RTF = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 26U, STAT_REG_OFFSET, 11U),      /*!< receiver timeout interrupt and flag */
    USART_INT_AMIE_AMF = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 14U, STAT_REG_OFFSET, 17U),      /*!< address match interrupt and flag */
    USART_INT_PERRIE_PERR = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 8U, STAT_REG_OFFSET, 0U),     /*!< parity error interrupt and flag */
    USART_INT_TBEIE_TBE = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 7U, STAT_REG_OFFSET, 7U),       /*!< transmitter buffer empty interrupt and flag */
    USART_INT_TCIE_TC = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 6U, STAT_REG_OFFSET, 6U),         /*!< transmission complete interrupt and flag */
    USART_INT_RBNEIE_RBNE = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 5U, STAT_REG_OFFSET, 5U),     /*!< read data buffer not empty interrupt and flag */
    USART_INT_RBNEIE_ORERR = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 5U, STAT_REG_OFFSET, 3U),    /*!< read data buffer not empty interrupt and overrun error flag */
    USART_INT_IDLEIE_IDLEF = USART_REGIDX_BIT2(CTL0_REG_OFFSET, 4U, STAT_REG_OFFSET, 4U),    /*!< IDLE line detected interrupt and flag */
    /* interrupt flags in CTL1 register */
    USART_INT_LBDIE_LBDF = USART_REGIDX_BIT2(CTL1_REG_OFFSET, 6U, STAT_REG_OFFSET, 8U),      /*!< LIN break detected interrupt and flag */
    /* interrupt flags in CTL2 register */
    USART_INT_WUIE_WUF = USART_REGIDX_BIT2(CTL2_REG_OFFSET, 22U, STAT_REG_OFFSET, 20U),      /*!< wakeup from deep-sleep mode interrupt and flag */
    USART_INT_CTSIE_CTSF = USART_REGIDX_BIT2(CTL2_REG_OFFSET, 10U, STAT_REG_OFFSET, 10U),    /*!< CTS interrupt and flag */
    USART_INT_ERRIE_NERR = USART_REGIDX_BIT2(CTL2_REG_OFFSET, 0U, STAT_REG_OFFSET, 2U),      /*!< error interrupt and noise error flag */
    USART_INT_ERRIE_ORERR = USART_REGIDX_BIT2(CTL2_REG_OFFSET, 0U, STAT_REG_OFFSET, 3U),     /*!< error interrupt and overrun error */
    USART_INT_ERRIE_FERR = USART_REGIDX_BIT2(CTL2_REG_OFFSET, 0U, STAT_REG_OFFSET, 1U),      /*!< error interrupt and frame error flag */
    /* interrupt flags in RFCS register */
    USART_INT_RFFIE_RFFINT = USART_REGIDX_BIT2(RFCS_REG_OFFSET, 9U, RFCS_REG_OFFSET, 15U),   /*!< receive FIFO full interrupt and flag */
}usart_interrupt_flag_enum;

/* USART interrupt enable or disable */
typedef enum
{
    /* interrupt in CTL0 register */
    USART_INT_EBIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 27U),         /*!< end of block interrupt */
    USART_INT_RTIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 26U),         /*!< receiver timeout interrupt */
    USART_INT_AMIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 14U),         /*!< address match interrupt */
    USART_INT_PERRIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 8U),        /*!< parity error interrupt */
    USART_INT_TBEIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 7U),         /*!< transmitter buffer empty interrupt */
    USART_INT_TCIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 6U),          /*!< transmission complete interrupt */
    USART_INT_RBNEIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 5U),        /*!< read data buffer not empty interrupt and overrun error interrupt */
    USART_INT_IDLEIE = USART_REGIDX_BIT(CTL0_REG_OFFSET, 4U),        /*!< IDLE line detected interrupt */
    /* interrupt in CTL1 register */
    USART_INT_LBDIE = USART_REGIDX_BIT(CTL1_REG_OFFSET, 6U),         /*!< LIN break detected interrupt */
    /* interrupt in CTL2 register */
    USART_INT_WUIE = USART_REGIDX_BIT(CTL2_REG_OFFSET, 22U),         /*!< wakeup from deep-sleep mode interrupt */
    USART_INT_CTSIE = USART_REGIDX_BIT(CTL2_REG_OFFSET, 10U),        /*!< CTS interrupt */
    USART_INT_ERRIE = USART_REGIDX_BIT(CTL2_REG_OFFSET, 0U),         /*!< error interrupt */
    /* interrupt in RFCS register */
    USART_INT_RFFIE = USART_REGIDX_BIT(RFCS_REG_OFFSET, 9U),         /*!< receive FIFO full interrupt */
}usart_interrupt_enum;

/* USART receiver configure */
#define CTL0_REN(regval)              (BIT(2) & ((uint32_t)(regval) << 2))
#define USART_RECEIVE_ENABLE          CTL0_REN(1)                    /*!< enable receiver */
#define USART_RECEIVE_DISABLE         CTL0_REN(0)                    /*!< disable receiver */

/* USART transmitter configure */
#define CTL0_TEN(regval)              (BIT(3) & ((uint32_t)(regval) << 3))
#define USART_TRANSMIT_ENABLE         CTL0_TEN(1)                    /*!< enable transmitter */
#define USART_TRANSMIT_DISABLE        CTL0_TEN(0)                    /*!< disable transmitter */

/* USART parity mode definitions */
#define CTL0_PM(regval)               (BITS(9,10) & ((uint32_t)(regval) << 9))
#define USART_PM_NONE                 CTL0_PM(0)                     /*!< no parity */
#define USART_PM_EVEN                 CTL0_PM(2)                     /*!< even parity */
#define USART_PM_ODD                  CTL0_PM(3)                     /*!< odd parity */

/* USART wakeup method in mute mode */
#define CTL0_WM(regval)               (BIT(11) & ((uint32_t)(regval) << 11))
#define USART_WM_IDLE                 CTL0_WM(0)                     /*!< idle line */
#define USART_WM_ADDR                 CTL0_WM(1)                     /*!< address mark */

/* USART word length definitions */
#define CTL0_WL(regval)               (BIT(12) & ((uint32_t)(regval) << 12))
#define USART_WL_8BIT                 CTL0_WL(0)                     /*!< 8 bits */
#define USART_WL_9BIT                 CTL0_WL(1)                     /*!< 9 bits */

/* USART oversample mode */
#define CTL0_OVSMOD(regval)           (BIT(15) & ((uint32_t)(regval) << 15))
#define USART_OVSMOD_8                CTL0_OVSMOD(1)                 /*!< oversampling by 8 */
#define USART_OVSMOD_16               CTL0_OVSMOD(0)                 /*!< oversampling by 16 */

/* USART address detection mode */
#define CTL1_ADDM(regval)             (BIT(4) & ((uint32_t)(regval) << 4))
#define USART_ADDM_4BIT               CTL1_ADDM(0)                   /*!< 4-bit address detection */
#define USART_ADDM_FULLBIT            CTL1_ADDM(1)                   /*!< full-bit address detection */

/* USART LIN break detection length */
#define CTL1_LBLEN(regval)            (BIT(5) & ((uint32_t)(regval) << 5))
#define USART_LBLEN_10B               CTL1_LBLEN(0)                  /*!< 10 bits break detection */
#define USART_LBLEN_11B               CTL1_LBLEN(1)                  /*!< 11 bits break detection */

/* USART last bit clock pulse */
#define CTL1_CLEN(regval)             (BIT(8) & ((uint32_t)(regval) << 8))
#define USART_CLEN_NONE               CTL1_CLEN(0)                   /*!< clock pulse of the last data bit (MSB) is not output to the CK pin */
#define USART_CLEN_EN                 CTL1_CLEN(1)                   /*!< clock pulse of the last data bit (MSB) is output to the CK pin */

/* USART clock phase */
#define CTL1_CPH(regval)              (BIT(9) & ((uint32_t)(regval) << 9))
#define USART_CPH_1CK                 CTL1_CPH(0)                    /*!< first clock transition is the first data capture edge */
#define USART_CPH_2CK                 CTL1_CPH(1)                    /*!< second clock transition is the first data capture edge */

/* USART clock polarity */
#define CTL1_CPL(regval)              (BIT(10) & ((uint32_t)(regval) << 10))
#define USART_CPL_LOW                 CTL1_CPL(0)                    /*!< steady low value on CK pin */
#define USART_CPL_HIGH                CTL1_CPL(1)                    /*!< steady high value on CK pin */

/* USART stop bits definitions */
#define CTL1_STB(regval)              (BITS(12,13) & ((uint32_t)(regval) << 12))
#define USART_STB_1BIT                CTL1_STB(0)                    /*!< 1 bit */
#define USART_STB_0_5BIT              CTL1_STB(1)                    /*!< 0.5 bit */
#define USART_STB_2BIT                CTL1_STB(2)                    /*!< 2 bits */
#define USART_STB_1_5BIT              CTL1_STB(3)                    /*!< 1.5 bits */

/* USART data is transmitted/received with the LSB/MSB first */
#define CTL1_MSBF(regval)             (BIT(19) & ((uint32_t)(regval) << 19))
#define USART_MSBF_LSB                CTL1_MSBF(0)                   /*!< LSB first */
#define USART_MSBF_MSB                CTL1_MSBF(1)                   /*!< MSB first */

/* USART auto baud rate detection mode bits definitions */
#define CTL1_ABDM(regval)             (BITS(21,22) & ((uint32_t)(regval) << 21))
#define USART_ABDM_FTOR               CTL1_ABDM(0)                   /*!< falling edge to rising edge measurement */
#define USART_ABDM_FTOF               CTL1_ABDM(1)                   /*!< falling edge to falling edge measurement */

/* USART IrDA low-power enable */
#define CTL2_IRLP(regval)             (BIT(2) & ((uint32_t)(regval) << 2))
#define USART_IRLP_LOW                CTL2_IRLP(1)                   /*!< low-power */
#define USART_IRLP_NORMAL             CTL2_IRLP(0)                   /*!< normal */

/* DMA enable for reception */
#define CTL2_DENR(regval)             (BIT(6) & ((uint32_t)(regval) << 6))
#define USART_DENR_ENABLE             CTL2_DENR(1)                   /*!< enable for reception */
#define USART_DENR_DISABLE            CTL2_DENR(0)                   /*!< disable for reception */

/* DMA enable for transmission */
#define CTL2_DENT(regval)             (BIT(7) & ((uint32_t)(regval) << 7))
#define USART_DENT_ENABLE             CTL2_DENT(1)                   /*!< enable for transmission */
#define USART_DENT_DISABLE            CTL2_DENT(0)                   /*!< disable for transmission */

/* USART RTS hardware flow control configure */
#define CTL2_RTSEN(regval)            (BIT(8) & ((uint32_t)(regval) << 8))
#define USART_RTS_ENABLE              CTL2_RTSEN(1)                  /*!< RTS hardware flow control enabled */
#define USART_RTS_DISABLE             CTL2_RTSEN(0)                  /*!< RTS hardware flow control disabled */

/* USART CTS hardware flow control configure */
#define CTL2_CTSEN(regval)            (BIT(9) & ((uint32_t)(regval) << 9))
#define USART_CTS_ENABLE              CTL2_CTSEN(1)                  /*!< CTS hardware flow control enabled */
#define USART_CTS_DISABLE             CTL2_CTSEN(0)                  /*!< CTS hardware flow control disabled */

/* USART one sample bit method configure */
#define CTL2_OSB(regval)              (BIT(11) & ((uint32_t)(regval) << 11))
#define USART_OSB_1BIT                CTL2_OSB(1)                    /*!< 1 sample bit */
#define USART_OSB_3BIT                CTL2_OSB(0)                    /*!< 3 sample bits */

/* USART driver enable polarity mode */
#define CTL2_DEP(regval)              (BIT(15) & ((uint32_t)(regval) << 15))
#define USART_DEP_HIGH                CTL2_DEP(0)                    /*!< DE signal is active high */
#define USART_DEP_LOW                 CTL2_DEP(1)                    /*!< DE signal is active low */

/* USART wakeup mode from deep-sleep mode */
#define CTL2_WUM(regval)              (BITS(20,21) & ((uint32_t)(regval) << 20))
#define USART_WUM_ADDR                CTL2_WUM(0)                    /*!< WUF active on address match */
#define USART_WUM_STARTB              CTL2_WUM(2)                    /*!< WUF active on start bit */
#define USART_WUM_RBNE                CTL2_WUM(3)                    /*!< WUF active on RBNE */

/* function declarations */
/* initialization functions */
/* reset USART */
void usart_deinit(uint32_t usart_periph);
/* configure USART baud rate value */
void usart_baudrate_set(uint32_t usart_periph, uint32_t baudval);
/* configure USART parity function */
void usart_parity_config(uint32_t usart_periph, uint32_t paritycfg);
/* configure USART word length */
void usart_word_length_set(uint32_t usart_periph, uint32_t wlen);
/* configure USART stop bit length */
void usart_stop_bit_set(uint32_t usart_periph, uint32_t stblen);

/* USART normal mode communication */
/* enable USART */
void usart_enable(uint32_t usart_periph);
/* disable USART */
void usart_disable(uint32_t usart_periph);
/* configure USART transmitter */
void usart_transmit_config(uint32_t usart_periph, uint32_t txconfig);
/* configure USART receiver */
void usart_receive_config(uint32_t usart_periph, uint32_t rxconfig);
/* USART transmit data function */
void usart_data_transmit(uint32_t usart_periph, uint32_t data);
/* USART receive data function */
uint16_t usart_data_receive(uint32_t usart_periph);
/* data is transmitted/received with the LSB/MSB first */
void usart_data_first_config(uint32_t usart_periph, uint32_t msbf);
/* configure USART inverted */
void usart_invert_config(uint32_t usart_periph, usart_invert_enum invertpara);
/* overrun function is enabled */
void usart_overrun_enable(uint32_t usart_periph);
/* overrun function is disabled */
void usart_overrun_disable(uint32_t usart_periph);
/* configure the USART oversample mode */
void usart_oversample_config(uint32_t usart_periph, uint32_t oversamp);
/* sample bit method configure */
void usart_sample_bit_config(uint32_t usart_periph, uint32_t osb);

/* auto baud rate detection */
/* auto baud rate detection enable */
void usart_autobaud_detection_enable(uint32_t usart_periph);
/* auto baud rate detection disable */
void usart_autobaud_detection_disable(uint32_t usart_periph);
/* auto baud rate detection mode configure */
void usart_autobaud_detection_mode_config(uint32_t usart_periph, uint32_t abdmod);

/* multi-processor communication */
/* enable mute mode */
void usart_mute_mode_enable(uint32_t usart_periph);
/* disable mute mode */
void usart_mute_mode_disable(uint32_t usart_periph);
/* configure wakeup method in mute mode */
void usart_mute_mode_wakeup_config(uint32_t usart_periph, uint32_t wmethod);
/* address detection mode configure */
void usart_address_detection_mode_config(uint32_t usart_periph, uint32_t addmod);
/* configure address of the USART */
void usart_address_config(uint32_t usart_periph, uint8_t usartaddr);
/* enable receiver timeout */
void usart_receiver_timeout_enable(uint32_t usart_periph);
/* disable receiver timeout */
void usart_receiver_timeout_disable(uint32_t usart_periph);
/* configure receiver timeout threshold */
void usart_receiver_timeout_config(uint32_t usart_periph, uint32_t rtimeout);

/* LIN mode communication */
/* LIN mode enable */
void usart_lin_mode_enable(uint32_t usart_periph);
/* LIN mode disable */
void usart_lin_mode_disable(uint32_t usart_periph);
/* LIN break detection length */
void usart_lin_break_dection_length_config(uint32_t usart_periph, uint32_t lblen);

/* half-duplex communication */
/* half-duplex enable */
void usart_halfduplex_enable(uint32_t usart_periph);
/* half-duplex disable */
void usart_halfduplex_disable(uint32_t usart_periph);

/* synchronous communication */
/* clock enable */
void usart_clock_enable(uint32_t usart_periph);
/* clock disable*/
void usart_clock_disable(uint32_t usart_periph);
/* configure USART synchronous mode parameters */
void usart_synchronous_clock_config(uint32_t usart_periph, uint32_t clen, uint32_t cph, uint32_t cpl);

/* smartcard communication */
/* smartcard mode enable */
void usart_smartcard_mode_enable(uint32_t usart_periph);
/* smartcard mode disable */
void usart_smartcard_mode_disable(uint32_t usart_periph);
/* NACK enable in smartcard mode */
void usart_smartcard_mode_nack_enable(uint32_t usart_periph);
/* NACK disable in smartcard mode */
void usart_smartcard_mode_nack_disable(uint32_t usart_periph);
/* guard time value configure in smartcard mode */
void usart_guard_time_config(uint32_t usart_periph, uint32_t guat);
/* block length configure */
void usart_block_length_config(uint32_t usart_periph, uint32_t bl);
/* smartcard auto-retry number configure */
void usart_smartcard_autoretry_config(uint32_t usart_periph, uint32_t scrtnum);
/* early NACK enable */
void usart_smartcard_mode_early_nack_enable(uint32_t usart_periph);
/* early NACK disable */
void usart_smartcard_mode_early_nack_disable(uint32_t usart_periph);

/* IrDA communication */
/* enable IrDA mode */
void usart_irda_mode_enable(uint32_t usart_periph);
/* disable IrDA mode */
void usart_irda_mode_disable(uint32_t usart_periph);
/* configure IrDA low-power */
void usart_irda_lowpower_config(uint32_t usart_periph, uint32_t irlp);
/* configure the peripheral clock prescaler */
void usart_prescaler_config(uint32_t usart_periph, uint32_t psc);

/* hardware flow communication */
/* configure hardware flow control RTS */
void usart_hardware_flow_rts_config(uint32_t usart_periph, uint32_t rtsconfig);
/* configure hardware flow control CTS */
void usart_hardware_flow_cts_config(uint32_t usart_periph, uint32_t ctsconfig);

/* RS485 driver enable */
void usart_rs485_driver_enable(uint32_t usart_periph);
/* RS485 driver disable */
void usart_rs485_driver_disable(uint32_t usart_periph);
/* driver enable assertion time configure */
void usart_driver_assertime_config(uint32_t usart_periph, uint32_t deatime);
/* driver enable de-assertion time configure */
void usart_driver_deassertime_config(uint32_t usart_periph, uint32_t dedtime);
/* configure driver enable polarity mode */
void usart_depolarity_config(uint32_t usart_periph, uint32_t dep);

/* USART DMA */
/* configure USART DMA for reception */
void usart_dma_receive_config(uint32_t usart_periph, uint32_t dmacmd);
/* configure USART DMA for transmission */
void usart_dma_transmit_config(uint32_t usart_periph, uint32_t dmacmd);
/* disable DMA on reception error */
void usart_reception_error_dma_disable(uint32_t usart_periph);
/* enable DMA on reception error */
void usart_reception_error_dma_enable(uint32_t usart_periph);

/* USART be able to wake up the mcu from deep-sleep mode */
void usart_wakeup_enable(uint32_t usart_periph);
/* USART be not able to wake up the mcu from deep-sleep mode */
void usart_wakeup_disable(uint32_t usart_periph);
/* wakeup mode from deep-sleep mode */
void usart_wakeup_mode_config(uint32_t usart_periph, uint32_t wum);

/* USART receive FIFO */
/* receive FIFO enable */
void usart_receive_fifo_enable(uint32_t usart_periph);
/* receive FIFO disable */
void usart_receive_fifo_disable(uint32_t usart_periph);
/* read receive FIFO counter number */
uint8_t usart_receive_fifo_counter_number(uint32_t usart_periph);

/* enable USART interrupt */
void usart_interrupt_enable(uint32_t usart_periph, usart_interrupt_enum int_flag);
/* disable USART interrupt */
void usart_interrupt_disable(uint32_t usart_periph, usart_interrupt_enum int_flag);
/* enable USART command */
void usart_command_enable(uint32_t usart_periph, uint32_t cmdtype);
/* check USART status */
FlagStatus usart_flag_get(uint32_t usart_periph, usart_flag_enum flag);
/* clear USART status */
void usart_flag_clear(uint32_t usart_periph, usart_flag_enum flag);
/* get USART interrupt status */
FlagStatus usart_interrupt_flag_get(uint32_t usart_periph, usart_interrupt_flag_enum int_flag);
/* clear USART interrupt status */
void usart_interrupt_flag_clear(uint32_t usart_periph, usart_flag_enum flag);
#endif /* GD32F3X0_USART_H */
