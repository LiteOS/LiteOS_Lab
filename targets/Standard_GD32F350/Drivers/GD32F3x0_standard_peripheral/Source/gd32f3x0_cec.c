/*!
    \file  gd32f3x0_cec.c
    \brief CEC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#ifdef GD32F350

#include "gd32f3x0_cec.h"
#include "gd32f3x0_rcu.h"

/*!
    \brief      reset HDMI-CEC controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_deinit(void)
{
    rcu_periph_reset_enable(RCU_CECRST);
    rcu_periph_reset_disable(RCU_CECRST);
}

/*!
    \brief      configure signal free time,the signal free time counter start option,own address 
    \param[in]  sftmopt: signal free time counter start option
      \arg        CEC_SFT_START_STAOM: signal free time counter starts counting when STAOM is asserted
      \arg        CEC_SFT_START_LAST: signal free time counter starts automatically after transmission/reception end
    \param[in]  sft: signal free time
      \arg        CEC_SFT_PROTOCOL_PERIOD: the signal free time will perform as HDMI-CEC protocol description
      \arg        CEC_SFT_1POINT5_PERIOD: 1.5 nominal data bit periods
      \arg        CEC_SFT_2POINT5_PERIOD: 2.5 nominal data bit periods
      \arg        CEC_SFT_3POINT5_PERIOD: 3.5 nominal data bit periods
      \arg        CEC_SFT_4POINT5_PERIOD: 4.5 nominal data bit periods
      \arg        CEC_SFT_5POINT5_PERIOD: 5.5 nominal data bit periods
      \arg        CEC_SFT_6POINT5_PERIOD: 6.5 nominal data bit periods
      \arg        CEC_SFT_7POINT5_PERIOD: 7.5 nominal data bit periods
    \param[in]  address: own address
      \arg        CEC_OWN_ADDRESS_CLEAR: own address is cleared
      \arg        CEC_OWN_ADDRESSx(x=0..14): own address is x
    \param[out] none
    \retval     none
*/
void cec_init(uint32_t sftmopt, uint32_t sft, uint32_t address)
{
    uint32_t cfg;
    cfg = CEC_CFG;
    /* clear SFTMOPT bit,SFT[2:0] */
    cfg &= ~(CEC_CFG_SFTOPT | CEC_CFG_SFT);
    /* assign SFTMOPT bit,SFT[2:0] */
    cfg |= (sftmopt | sft);
    CEC_CFG = cfg;
    if(CEC_OWN_ADDRESS_CLEAR == address){
        CEC_CFG &= ~CEC_CFG_OWN_ADDRESS;
    }else{
        CEC_CFG |= address;
    }
}

/*!
    \brief      configure generate Error-bit when detected some abnormal situation or not,
                whether stop receive message when detected bit rising error
    \param[in]  broadcast:
      \arg        CEC_BROADCAST_ERROR_BIT_ON:generate Error-bit in broadcast
      \arg        CEC_BROADCAST_ERROR_BIT_OFF:do not generate Error-bit in broadcast
    \param[in]  singlecast_lbpe:
      \arg        CEC_LONG_PERIOD_ERROR_BIT_ON:generate Error-bit on long bit period error
      \arg        CEC_LONG_PERIOD_ERROR_BIT_OFF:do not generate Error-bit on long bit period error
    \param[in]  singlecast_bre:
      \arg        CEC_RISING_PERIOD_ERROR_BIT_ON:generate Error-bit on bit rising error
      \arg        CEC_RISING_PERIOD_ERROR_BIT_OFF:do not generate Error-bit on bit rising error
    \param[in]  rxbrestp:
      \arg        CEC_STOP_RISING_ERROR_BIT_ON: stop reception when detected bit rising error
      \arg        CEC_STOP_RISING_ERROR_BIT_OFF: do not stop reception when detected bit rising error
    \param[out] none
    \retval     none
*/
void cec_error_config(uint32_t broadcast, uint32_t singlecast_lbpe, uint32_t singlecast_bre, uint32_t rxbrestp)
{
    uint32_t cfg;
    cfg = CEC_CFG;
    /* clear BCNG bit, BPLEG bit, BREG bit */
    cfg &= ~(CEC_CFG_BCNG | CEC_CFG_BPLEG | CEC_CFG_BREG);
    /* assign BCNG bit, BPLEG bit, BREG bit */
    cfg |= (broadcast | singlecast_lbpe | singlecast_bre);
    CEC_CFG = cfg;
    if(CEC_STOP_RISING_ERROR_BIT_ON == rxbrestp){
        CEC_CFG |= CEC_CFG_BRES;
    }else{
        CEC_CFG &= ~CEC_CFG_BRES;
    }
}

/*!
    \brief      enable HDMI-CEC controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_enable(void)
{
    CEC_CTL |= CEC_CTL_CECEN;
}

/*!
    \brief      disable HDMI-CEC controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_disable(void)
{
    CEC_CTL &= ~CEC_CTL_CECEN;
}

/*!
    \brief      start CEC message transmission 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_transmission_start(void)
{
    CEC_CTL |= CEC_CTL_STAOM;
}

/*!
    \brief      end CEC message transmission
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_transmission_end(void)
{
    CEC_CTL |= CEC_CTL_ENDOM;
}

/*!
    \brief      enable CEC listen mode.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_listen_mode_enable(void)
{
    CEC_CFG |= CEC_CFG_LMEN;
}

/*!
    \brief      disable CEC listen mode.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_listen_mode_disable(void)
{
    CEC_CFG &= ~CEC_CFG_LMEN;
}

/*!
    \brief      configure and clear own address.the controller can be configured to multiple own address 
    \param[in]  address: own address
      \arg        CEC_OWN_ADDRESS_CLEAR: own address is cleared
      \arg        CEC_OWN_ADDRESSx(x=0..14): own address is x
    \param[out] none
    \retval     none
*/
void cec_own_address_config(uint32_t address)
{
    if(CEC_OWN_ADDRESS_CLEAR == address){
        CEC_CFG &= ~CEC_CFG_OWN_ADDRESS;
    } else {
        CEC_CFG |= address;
    }
}

/*!
    \brief      configure signal free time and the signal free time counter start option 
    \param[in]  sftmopt: signal free time counter start option
      \arg        CEC_SFT_START_STAOM: signal free time counter starts counting when STAOM is asserted
      \arg        CEC_SFT_START_LAST: signal free time counter starts automatically after transmission/reception end
    \param[in]  sft: signal free time
      \arg        CEC_SFT_PROTOCOL_PERIOD: the signal free time will perform as HDMI-CEC protocol description
      \arg        CEC_SFT_1POINT5_PERIOD: 1.5 nominal data bit periods
      \arg        CEC_SFT_2POINT5_PERIOD: 2.5 nominal data bit periods
      \arg        CEC_SFT_3POINT5_PERIOD: 3.5 nominal data bit periods
      \arg        CEC_SFT_4POINT5_PERIOD: 4.5 nominal data bit periods
      \arg        CEC_SFT_5POINT5_PERIOD: 5.5 nominal data bit periods
      \arg        CEC_SFT_6POINT5_PERIOD: 6.5 nominal data bit periods
      \arg        CEC_SFT_7POINT5_PERIOD: 7.5 nominal data bit periods
    \param[out] none
    \retval     none
*/
void cec_sft_config(uint32_t sftmopt, uint32_t sft)
{
    uint32_t cfg;
    cfg = CEC_CFG;
    /* clear SFTMOPT bit,SFT[2:0] */
    cfg &= ~(CEC_CFG_SFTOPT | CEC_CFG_SFT);
    /* assign SFTMOPT bit,SFT[2:0] */
    cfg |= (sftmopt | sft);
    CEC_CFG = cfg;
}

/*!
    \brief      configure generate Error-bit when detected some abnormal situation or not
    \param[in]  broadcast:
      \arg        CEC_BROADCAST_ERROR_BIT_ON:generate Error-bit in broadcast
      \arg        CEC_BROADCAST_ERROR_BIT_OFF:do not generate Error-bit in broadcast
    \param[in]  singlecast_lbpe:
      \arg        CEC_LONG_PERIOD_ERROR_BIT_ON:generate Error-bit on long bit period error
      \arg        CEC_LONG_PERIOD_ERROR_BIT_OFF:do not generate Error-bit on long bit period error
    \param[in]  singlecast_bre:
      \arg        CEC_RISING_PERIOD_ERROR_BIT_ON:generate Error-bit on bit rising error
      \arg        CEC_RISING_PERIOD_ERROR_BIT_OFF:do not generate Error-bit on bit rising error
    \param[out] none
    \retval     none
*/
void cec_generate_errorbit_config(uint32_t broadcast, uint32_t singlecast_lbpe, uint32_t singlecast_bre)
{
    uint32_t cfg;
    cfg = CEC_CFG;
    /* clear BCNG bit, RLBPEGEN bit, RBREGEN bit */
    cfg &= ~(CEC_CFG_BCNG | CEC_CFG_BPLEG | CEC_CFG_BREG);
    /* assign BCNG bit, RLBPEGEN bit, RBREGEN bit */
    cfg |= (broadcast | singlecast_lbpe | singlecast_bre);
    CEC_CFG = cfg;
}

/*!
    \brief      whether stop receive message when detected bit rising error
    \param[in]  rxbrestp:
      \arg        CEC_STOP_RISING_ERROR_BIT_ON: stop reception when detected bit rising error
      \arg        CEC_STOP_RISING_ERROR_BIT_OFF: do not stop reception when detected bit rising error
    \param[out] none
    \retval     none
*/
void cec_stop_receive_bre_config(uint32_t rxbrestp)
{
    if(CEC_STOP_RISING_ERROR_BIT_ON == rxbrestp){
        CEC_CFG |= CEC_CFG_BRES;
    } else {
        CEC_CFG &= ~CEC_CFG_BRES;
    }
}

/*!
    \brief      enable reception bit timing tolerance
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_reception_tolerance_enable(void)
{
    CEC_CFG |= CEC_CFG_RTOL;
}

/*!
    \brief      disable reception bit timing tolerance
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cec_reception_tolerance_disable(void)
{
    CEC_CFG &= ~CEC_CFG_RTOL;
}

/*!
    \brief      send a data by the CEC peripheral
    \param[in]  data: the data to transmit
    \param[out] none
    \retval     none
*/
void cec_data_send(uint8_t data)
{
    CEC_TDATA = (uint32_t)data;
}

/*!
    \brief      receive a data by the CEC peripheral
    \param[in]  data: the data to receive
    \param[out] none
    \retval     none
*/
uint8_t cec_data_receive(void)
{
    return (uint8_t)CEC_RDATA;
}

/*!
    \brief      get CEC int flag
    \param[in]  flag:  specify which flag
      \arg        CEC_INTF_BR: Rx-byte data received
      \arg        CEC_INTF_REND: end of reception
      \arg        CEC_INTF_RO: RX overrun
      \arg        CEC_INTF_BRE: bit rising error
      \arg        CEC_INTF_BPSE: short bit period error
      \arg        CEC_INTF_BPLE: long bit period error
      \arg        CEC_INTF_RAE: Rx ACK error
      \arg        CEC_INTF_ARBF: arbitration lost
      \arg        CEC_INTF_TBR: Tx-byte data request
      \arg        CEC_INTF_TEND: transmission successfully end
      \arg        CEC_INTF_TU: Tx data buffer underrun
      \arg        CEC_INTF_TERR: Tx-error
      \arg        CEC_INTF_TAERR: Tx ACK error flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus cec_interrupt_flag_get(uint32_t flag)
{
    if(((uint32_t)RESET != (CEC_INTF & flag)) && ((uint32_t)RESET != (CEC_INTEN & flag))){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      get CEC status
    \param[in]  flag:  specify which flag
      \arg        CEC_INTF_BR: Rx-byte data received
      \arg        CEC_INTF_REND: end of reception
      \arg        CEC_INTF_RO: RX overrun
      \arg        CEC_INTF_BRE: bit rising error
      \arg        CEC_INTF_BPSE: short bit period error
      \arg        CEC_INTF_BPLE: long bit period error
      \arg        CEC_INTF_RAE: Rx ACK error
      \arg        CEC_INTF_ARBF: arbitration lost
      \arg        CEC_INTF_TBR: Tx-byte data request
      \arg        CEC_INTF_TEND: transmission successfully end
      \arg        CEC_INTF_TU: Tx data buffer underrun
      \arg        CEC_INTF_TERR: Tx-error
      \arg        CEC_INTF_TAERR: Tx ACK error flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus cec_flag_get(uint32_t status)
{
    if(CEC_INTF & status){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear CEC int flag and status
    \param[in]  flag:  specify which flag
      \arg        CEC_INTF_BRC: clear Rx-byte data received flag
      \arg        CEC_INTF_RENDC: clear end of reception flag
      \arg        CEC_INTF_ROC: clear RX overrun flag
      \arg        CEC_INTF_BREC: clear bit rising error flag
      \arg        CEC_INTF_BPSEC: clear short bit period error flag
      \arg        CEC_INTF_BPLEC: clear long bit period error flag
      \arg        CEC_INTF_RAEC: clear Rx ACK error flag
      \arg        CEC_INTF_ARBFC: clear arbitration lost flag
      \arg        CEC_INTF_TBRC: clear Tx-byte data request flag
      \arg        CEC_INTF_TENDC: clear transmission successfully end flag
      \arg        CEC_INTF_TUC: clear Tx data buffer underrun flag
      \arg        CEC_INTF_TERRC: clear Tx-error flag
      \arg        CEC_INTF_TAERRC: clear Tx ACK error flag
    \param[out] none
    \retval     none
*/
void cec_interrupt_flag_clear(uint32_t flag)
{
    CEC_INTF = flag;
}

/*!
    \brief      enable interrupt
    \param[in]  flag: specify which flag
      \arg        CEC_INTEN_BRIE: enable Rx-byte data received interrupt
      \arg        CEC_INTEN_RENDIE: enable end of reception interrupt
      \arg        CEC_INTEN_ROIE: enable RX overrun interrupt
      \arg        CEC_INTEN_BREIE: enable bit rising error interrupt
      \arg        CEC_INTEN_BPSEIE: enable short bit period error interrupt
      \arg        CEC_INTEN_BPLEIE: enable long bit period error interrupt
      \arg        CEC_INTEN_RAEIE: enable Rx ACK error interrupt
      \arg        CEC_INTEN_ARBFIE: enable arbitration lost interrupt
      \arg        CEC_INTEN_TBRIE: enable Tx-byte data request interrupt
      \arg        CEC_INTEN_TENDIE: enable transmission successfully end interrupt
      \arg        CEC_INTEN_TUIE: enable Tx data buffer underrun interrupt
      \arg        CEC_INTEN_TERRIE: enable Tx-error interrupt 
      \arg        CEC_INTEN_TAERRIE: enable Tx ACK error  interrupt
    \param[out] none
    \retval     none
*/
void cec_interrupt_enable(uint32_t flag)
{
    CEC_INTEN |= flag;
}

/*!
    \brief      disable interrupt
    \param[in]  flag: specify which flag
      \arg        CEC_INTEN_BRIE: disable Rx-byte data received interrupt
      \arg        CEC_INTEN_RENDIE: disable end of reception interrupt
      \arg        CEC_INTEN_ROIE: disable RX overrun interrupt
      \arg        CEC_INTEN_BREIE: disable bit rising error interrupt
      \arg        CEC_INTEN_BPSEIE: disable short bit period error interrupt
      \arg        CEC_INTEN_BPLEIE: disable long bit period error interrupt
      \arg        CEC_INTEN_RAEIE: disable Rx ACK error interrupt
      \arg        CEC_INTEN_ARBFIE: disable arbitration lost interrupt
      \arg        CEC_INTEN_TBRIE: disable Tx-byte data request interrupt
      \arg        CEC_INTEN_TENDIE: disable transmission successfully end interrupt
      \arg        CEC_INTEN_TUIE: disable Tx data buffer underrun interrupt
      \arg        CEC_INTEN_TERRIE: disable Tx-error interrupt 
      \arg        CEC_INTEN_TAERRIE: disable Tx ACK error  interrupt
    \param[out] none
    \retval     none
*/
void cec_interrupt_disable(uint32_t flag)
{
    CEC_INTEN &= ~flag;
}

#endif /* GD32F350 */
