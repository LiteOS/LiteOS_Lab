/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   SDRAM parameter configuration
* | Info        :
*                SDRAM parameter configuration
*----------------
* |	This version:   V1.0
* | Date        :   2019-2-28
* | Info        :

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __BSP_SDRAM_H
#define __BSP_SDRAM_H

#include "fmc.h"
#include "stm32f4xx_hal.h"
/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM324x9I_EVAL
  * @{
  */
    
/** @defgroup STM324x9I_EVAL_SDRAM
  * @{
  */    

/** @defgroup STM324x9I_EVAL_SDRAM_Exported_Types
  * @{
  */

/** 
  * @brief  SDRAM status structure definition  
  */     
#define   SDRAM_OK         0x00
#define   SDRAM_ERROR      0x01

/** @defgroup STM324x9I_EVAL_SDRAM_Exported_Constants
  * @{
  */ 
#define SDRAM_DEVICE_ADDR  ((uint32_t)0xD0000000)
#define SDRAM_DEVICE_SIZE  ((uint32_t)0x800000)  /* SDRAM device size in MBytes */ 

#define REFRESH_COUNT      ((uint32_t)0x0569)   /* SDRAM refresh counter (90Mhz SD clock) */
   
#define SDRAM_TIMEOUT     ((uint32_t)0xFFFF) 

 
/**
  * @}
  */ 
  
/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 
/**
  * @}
  */ 
  
/** @defgroup STM324x9I_EVAL_SDRAM_Exported_Macro
  * @{
  */  
/**
  * @}
  */ 
   
/** @defgroup STM324x9I_EVAL_SDRAM_Exported_Functions
  * @{
  */  
void BSP_SDRAM_Init(void);
void    BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount);
uint8_t BSP_SDRAM_ReadData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_ReadData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_WriteData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_WriteData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd);
void    BSP_SDRAM_DMA_IRQHandler(void);  

#endif
