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
#include <string.h>
#include <stdlib.h>
#include <osport.h>


#include <app_main.h>

#include <los_dev.h>


#include <gps.h>

static los_dev_t  s_sensor_dev = NULL;



//do the light intensity report
static s32_t gps_report(u8_t *buf, s32_t buflen)
{

    s32_t ret = 0;
    u8_t  databuf[128];
    //sample light intensity and report
#if 0
      u32_t  value  =0;
	  HAL_UART_Receive_IT(&huart3,gps_uart,1000);
		NMEA_BDS_GPRMC_Analysis(&gpsmsg,(uint8_t*)gps_uart);	//?????
		Longitude=(float)((float)gpsmsg.longitude_bd/100000);	
		printf("Longitude:%.5f %lc     \r\n",Longitude,gpsmsg.ewhemi_bd);
		Latitude=(float)((float)gpsmsg.latitude_bd/100000);
		printf("Latitude:%.5f %1c   \r\n",Latitude,gpsmsg.nshemi_bd);
#else
	    ret = los_dev_read(s_sensor_dev,0,databuf,128,1000);
	    printf("read :%d bytes\n\r",ret);
#endif
    return 0;
}


static void gps_device_init(void)
{

#if 0
	HAL_UART_Transmit(&huart3, "$CCMSG,GGA,1,0,*19\r\n", 20, 200);
    HAL_UART_Transmit(&huart3, "$CCMSG,GSA,1,0,*0D\r\n", 20, 200);
	HAL_UART_Transmit(&huart3, "$CCMSG,GSV,1,0,*1A\r\n", 20, 200);
#else
    const char *cmd;
    cmd = "$CCMSG,GGA,1,0,*19\r\n";  
    los_dev_write(s_sensor_dev,0,(u8_t *)cmd,strlen(cmd),200);
    
    cmd = "$CCMSG,GSA,1,0,*0D\r\n";  
    los_dev_write(s_sensor_dev,0,(u8_t *)cmd,strlen(cmd),200);
    
    cmd = "$CCMSG,GSV,1,0,*1A\r\n";  
    los_dev_write(s_sensor_dev,0,(u8_t *)cmd,strlen(cmd),200);

#endif

}

bool_t app_gps_report()
{

#if 0


#else
    bool_t ret= false;
    
    //do the sensor module init
    extern bool_t sensor_uart_init(s32_t baudrate);
    sensor_uart_init(9600);
    
    s_sensor_dev = los_dev_open("sensor_uart",O_RDWR);
    if(NULL != s_sensor_dev)
    {
        ret = app_register("appgps",en_app_direction_report,en_app_msgid_gps,\
          gps_report,10);
    }

    gps_device_init();

    return ret;
#endif
}









