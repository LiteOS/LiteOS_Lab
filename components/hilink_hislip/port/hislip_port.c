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
#include <stdlib.h>
#include "hislip.h"
#include "hilink_module_op.h"
#include "dev_com_struct.h"
#include "hilink.h"
#include "dev_hw.h"
#include "bsp_led.h"

#include "usart.h"

/* add your sdk include files here if needed */
#include "los_sys.h"
#include "los_sys.ph"
#include "los_tick.h"
#include "los_task.h"

/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*****************************************************************************
 Function    : hlk_notify_error_info
 Description : give out some error message to user. 
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void hlk_notify_error_info(hislip_u32 hlkerrno)
{
	/* add your implement code here */
    //#error ERRORS please implement "hlk_notify_error_info()"
    return ;
}


/*****************************************************************************
 Function    : hislip_get_cur_time
 Description : get current time (ms)
 Input       : None
 Output      : None
 Return      : current time
 *****************************************************************************/
hislip_u64 hislip_get_cur_time(void)
{
	hislip_u64 ticks = 0x0;
	
	/* add your implement code here */
    //#error ERRORS please implement "hislip_get_cur_time()"
    ticks = osKernelGetTickCount() * (OS_SYS_MS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND);

    /*
        Note: we use tick count to do delay, so please make 
        the the tick count value's is 1000HZ per second
    */

    return ticks;
}



/*****************************************************************************
 Function    : hislip_uart_send_one_byte
 Description : send one byte data by uart, recommend send one byte in block mode
 Input       : data: send data
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/
hislip_s32 hislip_uart_send_one_byte(hislip_u8 data)
{
	//#error ERRORS please implement "hislip_uart_send_one_byte()".
	//printf("send data:%d\r\n", data);
    //uart_write(&data, 1, 0);
	return HISLIP_SUCC;
}

/*****************************************************************************
 Function    : device_init_devprofile_vals
 Description : init all service's keys values
 Input       : data: send data
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/

hislip_s32 device_init_devprofile_vals(void)
{
    hislip_u16 u16Fan_Max;
    hislip_u16 u16Fan_Min;
    hilink_msg_info_t msg_info;
    
   // #error ERRORS please implement "device_init_devprofile_vals()"
	
		//(void)hislip_gpio_read((uint8_t*)&msg_info.value, 1);
		msg_info.value = 0xabc;
		msg_info.svc_inst = LED_SVC_INSTANCE_ID;
		msg_info.map_id = LED_SVC_KEY_SWITCH_ID;
		hlk_update_local_val(msg_info.svc_inst,msg_info.map_id,msg_info.value,NULL);

    /*
        for example: we use a light on stm32 board as demo  

        step 1: get light's gpio's value, low as light off, hight as light on
            (void)hislip_gpio_read((uint8_t*)&msg_info.value, 1);
        step 2: set which service instance's key's value will update 
            msg_info.svc_inst = LED_SVC_INSTANCE_ID;
            msg_info.map_id = LED_SVC_KEY_SWITCH_ID;
        step 3: call hlk_update_local_val() to update the value, it will send to wifi module
        //by hilink&hislip 
            hlk_update_local_val(msg_info.svc_inst,msg_info.map_id,msg_info.value,NULL);
    */
    return 0;
}

/*****************************************************************************
 Function    : device_get_factory_infors
 Description : get some wifi module informations
 Input       : buf@ tha payload of the wifi info
                 len @ length that stored data in buf
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/

hislip_s32 device_get_factory_infors(hislip_s8 *buf, hislip_s32 len)
{
    /*
        NOTE:
            the buf contained 5 kinds of data in tlv format
            tlv : tag(2 bytes) len(1 byte) val(len bytes)
            data 1: wifi module's mac address
            data 2: wifi module's hardware version
            data 3: wifi module's software version
            data 4: ap's rssi value
            data 5: ap's name
    */

    /* todo: add your code here */
    
    return 0;
}

#define HISLIP_OTA

/*****************************************************************************
 Function    : hlk_req_check_upg
 Description : check the device if it need upgrade software .if need upgrade 
               software , call hislip_send_ack_msg(...) and it's para result
               should set to 0. else set result to none zero value
 Input       : cmd: HLK_CMD_DEV_QUPG
               buf: tlv format data , that contained version data
               len: data's lenght that stored in buf
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/
hislip_s32 hlk_req_check_upg(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8* buf, 
    hislip_u16 len)
{
    hislip_s32 upg = 0x0;//default for support upg
    hislip_s16 tag = 0x0;
	hislip_s32 offset = 0;
	hislip_u16 rlen = 0;
	
	/* get tag value */
    tag = hislip_parse_data_eabit(buf + offset, &rlen);
	printf("hlk_req_check_upg\r\n");
	
	if(HLK_TAG_DEV_SVER == tag)
	{
		//add you code here.
        //#error ERRORS please implement "device_init_devprofile_vals()"

	}
	
    hislip_send_ack_msg(seq, cmd, upg);
    return HISLIP_OK;
}

/*****************************************************************************
 Function    : hlk_start_upg
 Description : tell device to start software upgrade.if start upgrade 
               ok , call hislip_send_ack_msg(...) and it's para result
               should set to 0. else set result to none zero value
 Input       : cmd: HLK_CMD_DEV_SUPG
               buf: tlv format data , that contained software's bin size
               len: data's lenght that stored in buf
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/
hislip_s32 hlk_start_upg(hislip_u8 seq,
	hislip_u16 cmd,
    hislip_u8* buf,
    hislip_u16 len)
{
    hislip_u8 ret = HISLIP_OK;
	hislip_s16 tag = 0x0;
    hislip_s32 offset = 0;
    hislip_u16 rlen = 0;
	
    /* get tag value */
    tag = hislip_parse_data_eabit(buf + offset, &rlen);
	
	if(HLK_TAG_DEV_BIN_SIZE == tag)
	{
		//add you code here.

	}	
	
	ota_status = 1;
	
    hislip_send_ack_msg(seq, cmd, ret);
    return ret;
}

/*****************************************************************************
 Function    : hlk_rcv_upg_data
 Description : device recive software's bin data. if recived data's seq number
               is not sequenced , error happended, call 
               hislip_send_ack_msg(...) and it's para result
               should set to none 0. else set result to none zero value
 Input       : cmd: HLK_CMD_DEV_TRANSING
               buf: tlv format data , that contained software's bin data
               len: data's lenght that stored in buf
               seq: sequence number that current package
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/
hislip_s32 hlk_rcv_upg_data(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8* buf, 
    hislip_u16 len)
{
    hislip_u8 ret = HISLIP_OK;
    hislip_s16 tag = 0x0;
    hislip_s32 offset = 0;
	static hislip_s32 i = 0;
    hislip_u16 rlen = 0;
	
    /* get tag value */
    tag = hislip_parse_data_eabit(buf + offset, &rlen);
	
	if(HLK_TAG_DEV_BIN_DATA == tag)
	{
		//add you code here.

	}
	if(i == 0)
	{
		hislip_send_ack_msg(seq, cmd, ret);
	}
	i++;
	i = i%2;
    return ret;
}

/*****************************************************************************
 Function    : hlk_end_upg
 Description : upgrade finished process func. if recived checksum is not the
               same as local calculated, error happended, call 
               hislip_send_ack_msg(...) and it's para result
               should set to none 0. else set result to none zero value
 Input       : cmd: HLK_TAG_DEV_BIN_CHK
               buf: tlv format data , that contained software's checksum value
                    sha-256 vaule
                    v[257bytes]: status(1byte) 265bytes(sha-256)
               len: data's lenght that stored in buf
               seq: sequence number that current package
 Output      : None
 Return      : error reason/HISLIP_SUCC
 *****************************************************************************/

hislip_s32 hlk_end_upg(hislip_u8 seq, 	
	hislip_u16 cmd,
    hislip_u8* buf, 
    hislip_u16 len)
{
    hislip_u8 ret = HISLIP_OK;
	hislip_s16 tag = 0x0;
    hislip_s32 offset = 0;
    hislip_u16 rlen = 0;
	
    /* get tag value */
    tag = hislip_parse_data_eabit(buf + offset, &rlen);
	
	if(HLK_TAG_DEV_BIN_CHK == tag)
	{
		//add you code here.

	}
	
    hislip_send_ack_msg(seq, cmd, ret);
	ota_status = 0;
	
    return ret;
}


hislip_s32 hlk_tell_upg_info(hislip_u8 seq, 
	hislip_u16 cmd, 
    hislip_u8* buf, 
    hislip_u16 len)
{
    hislip_u8 ret = HISLIP_OK;
	
	//add you code here.
	 
    hislip_send_ack_msg(seq, cmd, ret);
    return ret;
}

hislip_s32 light_ctrl_func(hilink_msg_info_t* msg_info)
{
    hislip_s32 ret;

    /* todo: add your code here */
    // do hardware control , value is msg_info->value
    printf("ctrl %d %d %d\n", msg_info->svc_inst,msg_info->map_id,msg_info->value);
    if(msg_info->value)
    {
        LED_WHITE;
    }
    else
    {
        LED_RGBOFF;
    }

    hlk_update_local_val(msg_info->svc_inst,msg_info->map_id,msg_info->value,NULL);

    return ret;
}

hislip_s32 light_ctrl_func_1(hilink_msg_info_t* msg_info)
{
    hislip_s32 ret;

    /* todo: add your code here */
    // do hardware control , value is msg_info->value
    printf("ctrl111 %d %d %d\n", msg_info->svc_inst,msg_info->map_id,msg_info->value);

    hlk_update_local_val(msg_info->svc_inst,msg_info->map_id,msg_info->value,NULL);

    return ret;
}


