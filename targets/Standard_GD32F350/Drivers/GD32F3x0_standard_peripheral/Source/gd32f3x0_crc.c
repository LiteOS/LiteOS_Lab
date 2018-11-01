/*!
    \file  gd32f3x0_crc.c
    \brief CRC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0_crc.h"

/*!
    \brief      deinit CRC calculation unit
    \param[in]  none
    \param[out] none
    \retval     none
*/
void crc_deinit(void)
{
    CRC_IDATA = (uint32_t)0xFFFFFFFFU;
    CRC_DATA  = (uint32_t)0xFFFFFFFFU;
    CRC_FDATA = (uint32_t)0x00000000U;
    CRC_POLY  = (uint32_t)0x04C11DB7U;
    CRC_CTL   = CRC_CTL_RST;   
}

/*!
    \brief      enable the reverse operation of output data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void crc_reverse_output_data_enable(void)
{
    CRC_CTL &= (uint32_t)(~ CRC_CTL_REV_O);
    CRC_CTL |= (uint32_t)CRC_CTL_REV_O;
}

/*!
    \brief      disable the reverse operation of output data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void crc_reverse_output_data_disable(void)
{
    CRC_CTL &= (uint32_t)(~ CRC_CTL_REV_O);
}

/*!
    \brief      reset data register to the value of initializaiton data register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void crc_data_register_reset(void)
{
    CRC_CTL |= (uint32_t)CRC_CTL_RST;
}

/*!
    \brief      read the data register 
    \param[in]  none
    \param[out] none
    \retval     32-bit value of the data register
*/
uint32_t crc_data_register_read(void)
{
    uint32_t data;
    data = CRC_DATA;
    return (data);
}

/*!
    \brief      read the free data register
    \param[in]  none
    \param[out] none
    \retval     8-bit value of the free data register
*/
uint8_t crc_free_data_register_read(void)
{
    uint8_t fdata;
    fdata = (uint8_t)CRC_FDATA;
    return (fdata);
}

/*!
    \brief      write the free data register
    \param[in]  free_data: specify 8-bit data
    \param[out] none
    \retval     none
*/
void crc_free_data_register_write(uint8_t free_data)
{
    CRC_FDATA = (uint32_t)free_data;
}

/*!
    \brief      write the initializaiton data register
    \param[in]  init_data:specify 32-bit data
    \param[out] none
    \retval     none
*/
void crc_init_data_register_write(uint32_t init_data)
{
    CRC_IDATA = (uint32_t)init_data;
}

/*!
    \brief      configure the CRC input data function
    \param[in]  data_reverse: specify input data reverse function
      \arg        CRC_INPUT_DATA_NOT: input data is not reversed
      \arg        CRC_INPUT_DATA_BYTE: input data is reversed on 8 bits
      \arg        CRC_INPUT_DATA_HALFWORD: input data is reversed on 16 bits
      \arg        CRC_INPUT_DATA_WORD: input data is reversed on 32 bits
    \param[out] none
    \retval     none
*/
void crc_input_data_reverse_config(uint32_t data_reverse)
{
    CRC_CTL &= (uint32_t)(~CRC_CTL_REV_I);
    CRC_CTL |= (uint32_t)data_reverse;
}

/*!
    \brief      configure the CRC size of polynomial function
    \param[in]  poly_size: size of polynomial
      \arg        CRC_CTL_PS_32: 32-bit polynomial for CRC calculation
      \arg        CRC_CTL_PS_16: 16-bit polynomial for CRC calculation
      \arg        CRC_CTL_PS_8: 8-bit polynomial for CRC calculation
      \arg        CRC_CTL_PS_7: 7-bit polynomial for CRC calculation
    \param[out] none
    \retval     none
*/
void crc_polynomial_size_set(uint32_t poly_size)
{
    CRC_CTL &= (uint32_t)(~(CRC_CTL_PS));
    CRC_CTL |= (uint32_t)poly_size;
}

/*!
    \brief      configure the CRC polynomial value function
    \param[in]  poly: configurable polynomial value
    \param[out] none
    \retval     none
*/
void crc_polynomial_set(uint32_t poly)
{
    CRC_POLY &= (uint32_t)(~CRC_POLY_POLY);
    CRC_POLY = poly;
}

/*!
    \brief      CRC calculate a 32-bit data
    \param[in]  sdata: specify 32-bit data
    \param[out] none
    \retval     32-bit CRC calculate value
*/
uint32_t crc_single_data_calculate(uint32_t sdata)
{
    CRC_DATA = sdata;
    return(CRC_DATA);
}

/*!
    \brief      CRC calculate a 32-bit data array
    \param[in]  array: pointer to an array of 32 bit data words
    \param[in]  size: size of the array
    \param[out] none
    \retval     32-bit CRC calculate value
*/
uint32_t crc_block_data_calculate(uint32_t array[], uint32_t size)
{  
    uint32_t index;
    for(index = 0U; index < size; index++){
        CRC_DATA = array[index];
    }
    return (CRC_DATA);    
}
