/*----------------------------------------------------------------------------
 * Copyright (c) <2017-2018>, <Huawei Technologies Co., Ltd>
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
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "hislip_uart.h"
#include "hilink.h"
#include "bsp_led.h"

void hilink_example(void)
{
    hislip_u32 curtm = 0;
    hislip_u32 starttm = 0;
    hislip_u32 value= 0;

    
    hilink_env_init();
    LED_GPIO_Config();
    while(1)
    {
        hilink_main_process();
        /* netstatus == NET_ONLINE means wifi module already connected to hilink server */
        if(NET_ONLINE == hlk_get_loacal_net_status())
        {
            //todo: here just some example code, just for some funcs using reference 
            curtm = (hislip_u32)hislip_get_cur_time();
            if (starttm == 0)
            {
                starttm = curtm;
            }
            if ((curtm - starttm >= 2000))
            {
                //hlk_dev_reset();
                starttm = curtm;
                //testflg = 1;
				value++;
				if (value > 1300)
				{
					value = 100;
				}
                /* report one key value right now */
				//hlk_update_local_val(0x43,0x5c,value,NULL);
                
                /* just store one key value locally, until use DEV_REPORT_NOW*/
				//hlk_update_local_svcvals(0x43,0x5c,value,NULL, DEV_REPORT_LATER);
				
                /* store one key value locally, and report all service's all keys values right now */
				//hlk_update_local_svcvals(0x43,0x5c,value,NULL, DEV_REPORT_NOW);
				
				//hlk_get_factory_infos(NULL);
            }
        }
		LOS_TaskDelay(10);
    }
}
