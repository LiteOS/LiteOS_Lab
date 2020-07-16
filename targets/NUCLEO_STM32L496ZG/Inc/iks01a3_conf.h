/**
 ******************************************************************************
 * @file    iks01a3_conf.h
 * @author  MEMS Application Team
 * @brief   This file contains definitions for the MEMS components bus interfaces
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "stm32l4xx_hal.h"
#include "stm32l4xx_nucleo_bus.h"
#include "stm32l4xx_nucleo_errno.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IKS01A3_CONF_H__
#define __IKS01A3_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN 1 */
#define USE_IKS01A3_ENV_SENSOR_HTS221_0                1U
#define USE_IKS01A3_ENV_SENSOR_LPS22HH_0               1U
#define USE_IKS01A3_ENV_SENSOR_STTS751_0               1U

#define USE_IKS01A3_MOTION_SENSOR_LSM6DSO_0            1U
#define USE_IKS01A3_MOTION_SENSOR_LIS2DW12_0           1U
#define USE_IKS01A3_MOTION_SENSOR_LIS2MDL_0            1U
/* USER CODE END 1 */

#define IKS01A3_I2C_Init BSP_I2C1_Init
#define IKS01A3_I2C_DeInit BSP_I2C1_DeInit
#define IKS01A3_I2C_ReadReg BSP_I2C1_ReadReg
#define IKS01A3_I2C_WriteReg BSP_I2C1_WriteReg
#define IKS01A3_GetTick BSP_GetTick

#ifdef __cplusplus
}
#endif

#endif /* __IKS01A3_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
