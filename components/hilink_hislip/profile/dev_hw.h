#ifndef _DEV_HW_H
#define _DEV_HW_H

#include "dev_com_struct.h"

/* 
    Important Notice 1:
      service instance id macros that user defined 
      range: 0x40 ~ 0x7f or 0x80 ~ 0x1FFF
*/
#define LED_SVC_INSTANCE_ID (0x40)

/* 
    Important Notice 2:
        service's keys's id macros that user defined 
        range: 0x40 ~ 0x7f or 0x80 ~ 0x1FFF
*/
#define LED_SVC_KEY_SWITCH_ID (0x60)
#define LED_SVC_KEY_SWITCH_ID_1 (0x61)

/* 
    Important Notice 3:
        callback functions's declaration
        these callbacks will used to do the real actions acording
        its svc and svc's key .
*/

int light_ctrl_func(hilink_msg_info_t* msg_info);
int light_ctrl_func_1(hilink_msg_info_t* msg_info);


#endif

