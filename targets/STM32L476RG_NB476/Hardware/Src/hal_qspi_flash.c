/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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

#include "hal_qspi_flash.h"
#include "stm32l4xx.h"

#ifdef HAL_QSPI_MODULE_ENABLED

#define QSPI_FLASH_PAGESIZE                         256
#define QSPI_FLASH_SECTOR                           4096
#define QSPI_FLASH_ID                               0xEF4017
#define QSPI_FLASH_TOTAL_SIZE                       0x7FFFFF

#define QSPI_FLASH_WriteEnable                      0x06
#define QSPI_FLASH_WriteDisable                     0x04
#define QSPI_FLASH_ReadStatusReg                    0x05
#define QSPI_FLASH_WriteStatusReg                   0x01
#define QSPI_FLASH_ReadData                         0x03
#define QSPI_FLASH_FastReadData                     0x0B
#define QSPI_FLASH_FastReadDual                     0x3B
#define QSPI_FLASH_PageProgram                      0x02
#define QSPI_FLASH_BlockErase                       0xD8
#define QSPI_FLASH_SectorErase                      0x20
#define QSPI_FLASH_ChipErase                        0xC7
#define QSPI_FLASH_PowerDown                        0xB9
#define QSPI_FLASH_ReleasePowerDown                 0xAB
#define QSPI_FLASH_DeviceID                         0xAB
#define QSPI_FLASH_ManufactDeviceID                 0x90
#define QSPI_FLASH_JedecDeviceID                    0x9F
#define QSPI_FLASH_WIP_FLAG                         0x01
#define QSPI_FLASH_DUMMY_BYTE                       0xFF

#define CHOOSE_BIT_16                              16
#define CHOOSE_BIT_8                               8


#define CHECK_RET_RETURN(ret) \
    do \
    { \
        if ((ret) < 0) \
        { \
            return ret; \
        } \
    } while (0)

QSPI_HandleTypeDef hqspi;

/* This function is called by inner-HAL lib */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();
  
    /**QUADSPI GPIO Configuration    
    PB0     ------> QUADSPI_BK1_IO1
    PB1     ------> QUADSPI_BK1_IO0
    PB10     ------> QUADSPI_CLK
    PB11     ------> QUADSPI_BK1_NCS 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

/* This function is called by inner-HAL lib */
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();
  
    /**QUADSPI GPIO Configuration    
    PB0     ------> QUADSPI_BK1_IO1
    PB1     ------> QUADSPI_BK1_IO0
    PB10     ------> QUADSPI_CLK
    PB11     ------> QUADSPI_BK1_NCS 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}


/**
 * @brief	QSPI��������
 *
 * @param   instruction		Ҫ���͵�ָ��
 * @param   address			���͵���Ŀ�ĵ�ַ
 * @param   dummyCycles		��ָ��������
 * @param   instructionMode	ָ��ģʽ;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE
 * @param   addressMode		��ַģʽ; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
 * @param   addressSize		��ַ����;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
 * @param   dataMode		����ģʽ; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE
 *
 * @return  void
 */
uint32_t QSPI_Send_CMD(uint32_t instruction, uint32_t address, uint32_t dummyCycles, uint32_t instructionMode, uint32_t addressMode, uint32_t addressSize, uint32_t dataMode)
{
    QSPI_CommandTypeDef s_command;

    s_command.Instruction = instruction;                 	//ָ��
    s_command.Address = address;                            	//��ַ
    s_command.DummyCycles = dummyCycles;                   //���ÿ�ָ��������
    s_command.InstructionMode = instructionMode;				//ָ��ģʽ
    s_command.AddressMode = addressMode;   					//��ַģʽ
    s_command.AddressSize = addressSize;   					//��ַ����
    s_command.DataMode = dataMode;             				//����ģʽ
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;       	//ÿ�ζ�����ָ��
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; //�޽����ֽ�
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;           	//�ر�DDRģʽ
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	
    return HAL_QSPI_Command(&hqspi, &s_command, 5000);
}

uint8_t QSPI_Receive(const uint8_t * buf, uint32_t datalen)
{
    hqspi.Instance->DLR = datalen - 1;                       //Configuration data length
    if(HAL_QSPI_Receive(&hqspi, (uint8_t * )buf, 5000) == HAL_OK) return 0; //receive data

    else return 1;
}

uint8_t QSPI_Transmit(const int8_t * buf, uint32_t datalen)
{
    hqspi.Instance->DLR = datalen - 1;                        //Configuration data length
    if(HAL_QSPI_Transmit(&hqspi, (uint8_t * )buf, 5000) == HAL_OK) return 0; //send data

    else return 1;
}

static void prv_spi_flash_write_enable(void)
{
		QSPI_Send_CMD(QSPI_FLASH_WriteEnable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

static void prv_spi_flash_wait_write_end(void)
{
    uint8_t status = 0;


    /* Loop as long as the memory is busy with a write cycle */
    do
    {
        /* Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in status variable */
		QSPI_Send_CMD(QSPI_FLASH_ReadStatusReg, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(&status, 1);
			
    } while ((status & QSPI_FLASH_WIP_FLAG) == SET); /* Write in progress */

}

static int prv_spi_flash_write_page(const uint8_t* buf, uint32_t addr, int32_t len)
{
    int ret = 0;

    if(0 == len)
    {
        return 0;
    }

    prv_spi_flash_write_enable();					//Write enable
		
    QSPI_Send_CMD(QSPI_FLASH_PageProgram, addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Transmit((const int8_t *)buf, len);
    
		prv_spi_flash_wait_write_end();					   //Waiting for Writing to End
   
		return ret;
}

 int prv_spi_flash_erase_sector(uint32_t addr)
{
		//printf("fe:%x\r\n",addr);			//����flash�������,������  
		int ret = 0;
    prv_spi_flash_write_enable();                  //Write enable
    prv_spi_flash_wait_write_end();
    
		ret=QSPI_Send_CMD(QSPI_FLASH_SectorErase, addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);
    
		prv_spi_flash_wait_write_end();   				    //Waiting for Writing to End
	
		return ret;
}

void hal_spi_flash_config(void)
{
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 0;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = POSITION_VAL(0x1000000) - 1;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
   
}

int hal_spi_flash_erase(uint32_t addr, int32_t len)
{
    uint32_t begin;
    uint32_t end;
    int i;

    if (len < 0
        || addr > QSPI_FLASH_TOTAL_SIZE
        || addr + len > QSPI_FLASH_TOTAL_SIZE)
    {
        return -1;
    }

    begin = addr / QSPI_FLASH_SECTOR * QSPI_FLASH_SECTOR;
    end = (addr + len - 1) / QSPI_FLASH_SECTOR * QSPI_FLASH_SECTOR;

    for (i = begin; i <= end; i += QSPI_FLASH_SECTOR)
    {
        if (prv_spi_flash_erase_sector(i) == -1)
        {
            return -1;
        }
    }

    return 0;
}

int hal_spi_flash_write(const void* buf, int32_t len, uint32_t* location)
{
    const uint8_t* pbuf = (const uint8_t*)buf;
    int page_cnt = 0;
    int remain_cnt = 0;
    int temp = 0;
    uint32_t loc_addr;
    uint8_t addr = 0;
    uint8_t count = 0;
    int i;
    int ret = 0;

    if (NULL == pbuf
        || NULL == location
        || len < 0
        || *location > QSPI_FLASH_TOTAL_SIZE
        || len + *location > QSPI_FLASH_TOTAL_SIZE)
    {
        return -1;
    }

    loc_addr = *location;
    addr = loc_addr % QSPI_FLASH_PAGESIZE;
    count = QSPI_FLASH_PAGESIZE - addr;
    page_cnt = len / QSPI_FLASH_PAGESIZE;
    remain_cnt = len % QSPI_FLASH_PAGESIZE;

    if (addr == 0) /* addr is aligned to SPI_FLASH_PAGESIZE */
    {
        if (page_cnt == 0) /* len < SPI_FLASH_PAGESIZE */
        {
            ret = prv_spi_flash_write_page(pbuf, loc_addr, len);
            CHECK_RET_RETURN(ret);
        }
        else /* len > SPI_FLASH_PAGESIZE */
        {
            for (i = 0; i < page_cnt; ++i)
            {
                ret = prv_spi_flash_write_page(pbuf + i * QSPI_FLASH_PAGESIZE, loc_addr, QSPI_FLASH_PAGESIZE);
                CHECK_RET_RETURN(ret);
                loc_addr += QSPI_FLASH_PAGESIZE;
            }

            ret = prv_spi_flash_write_page(pbuf + page_cnt * QSPI_FLASH_PAGESIZE, loc_addr, remain_cnt);
            CHECK_RET_RETURN(ret);
        }
    }
    else /* addr is not aligned to SPI_FLASH_PAGESIZE */
    {
        if (page_cnt == 0) /* len < SPI_FLASH_PAGESIZE */
        {
            if (remain_cnt > count) /* (len + loc_addr) > SPI_FLASH_PAGESIZE */
            {
                temp = remain_cnt - count;

                ret = prv_spi_flash_write_page(pbuf, loc_addr, count);
                CHECK_RET_RETURN(ret);

                ret = prv_spi_flash_write_page(pbuf + count, loc_addr + count, temp);
                CHECK_RET_RETURN(ret);
            }
            else
            {
                ret = prv_spi_flash_write_page(pbuf, loc_addr, len);
                CHECK_RET_RETURN(ret);
            }
        }
        else /* len > SPI_FLASH_PAGESIZE */
        {
            len -= count;
            page_cnt = len / QSPI_FLASH_PAGESIZE;
            remain_cnt = len % QSPI_FLASH_PAGESIZE;

            ret = prv_spi_flash_write_page(pbuf, loc_addr, count);
            CHECK_RET_RETURN(ret);
            loc_addr += count;

            for (i = 0; i < page_cnt; ++i)
            {
                ret = prv_spi_flash_write_page(pbuf + count + i * QSPI_FLASH_PAGESIZE, loc_addr, QSPI_FLASH_PAGESIZE);
                CHECK_RET_RETURN(ret);
                loc_addr += QSPI_FLASH_PAGESIZE;
            }

            if (remain_cnt != 0)
            {
                ret = prv_spi_flash_write_page(pbuf + count + page_cnt * QSPI_FLASH_PAGESIZE, loc_addr, remain_cnt);
                CHECK_RET_RETURN(ret);
            }
        }
    }

    *location += len;
    return ret;
		
}

int hal_spi_flash_erase_write(const void* buf, int32_t len, uint32_t location)
{
    int ret = 0;

    ret = hal_spi_flash_erase(location, len);
    CHECK_RET_RETURN(ret);
    ret = hal_spi_flash_write(buf, len, &location);

    return ret;
}

int hal_spi_flash_read(void* buf, int32_t len, uint32_t location)
{
    int ret = 0;
    uint8_t* pbuf = (uint8_t*)buf;

    if (NULL == pbuf
        || len < 0
        || location > QSPI_FLASH_TOTAL_SIZE
        || len + location > QSPI_FLASH_TOTAL_SIZE)
    {
        return -1;
    }

		QSPI_Send_CMD(QSPI_FLASH_FastReadData, location, 8, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(buf, len);
		
    return ret;
		
}

int hal_spi_flash_get_id(void)
{
		
		uint8_t temp[2];
    uint16_t deviceid;
    QSPI_Send_CMD(QSPI_FLASH_JedecDeviceID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];
    return deviceid;
}


#endif /* HAL_QSPI_MODULE_ENABLED */
