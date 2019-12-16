/*----------------------------------------------------------------------------
 * Copyright (c) <2019>, <Huawei Technologies Co., Ltd>
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
/**
 *  DATE                AUTHOR      INSTRUCTION
 *  2019-10-17 19:50  zhangqianfu  The first version
 *  2019-12-06 19:50  zhuzongru    define interface
 *
 */
#ifndef LITEOS_LAB_IOT_LINK_NETWORK_LWM2M_LWM2M_AL_LWM2M_AL_H_
#define LITEOS_LAB_IOT_LINK_NETWORK_LWM2M_LWM2M_AL_LWM2M_AL_H_

#include <stdint.h>
#include <stdbool.h>


typedef enum
{
    LWM2M_OK                   = 0,
    LWM2M_ARG_INVALID          = -1,
    LWM2M_BUF_OVERFLOW         = -2,
    LWM2M_MSG_CONGEST          = -3,
    LWM2M_MALLOC_FAILED        = -4,
    LWM2M_RESOURCE_NOT_FOUND   = -5,
    LWM2M_RESOURCE_NOT_ENOUGH  = -6,
    LWM2M_CLIENT_UNREGISTERED  = -7,
    LWM2M_SOCKET_CREATE_FAILED = -8,
    LWM2M_ERR                  = -9,
    LWM2M_OBJECT_INSTANCE_EXISTED = -10,
    LWM2M_SUPPORT_SINGLE_INSTANCE_ONLY = -11,
    LWM2M_NOT_SUPPORTED = -12,
    LWM2M_NULL_POINTER = -13,
    LWM2M_COAP_400_BAD_REQUEST = -14
} lwm2m_errorcode_e;

typedef enum
{
    OBJ_SECURITY_ID = 0,
    OBJ_SERVER_ID,
    OBJ_ACCESS_CONTROL_ID,
    OBJ_DEVICE_ID,
    OBJ_CONNECTIVITY_MONITORING_ID,
    OBJ_FIRMWARE_UPDATE_ID,
    OBJ_LOCATION_ID,
    OBJ_CONNECTIVITY_STATISTICS_ID,
    OBJ_APP_DATA_ID = 19
} lwm2m_object_id_e;

typedef struct
{
    uint16_t            server_id;
    bool                is_bootstrap;
    int                 hold_off_time;  /* bootstrap hold off time for server initiated bootstrap */
    char               *server_ip;
    char               *server_port;
    char               *psk_id;
    char               *psk;
    uint16_t            psk_len;
} lwm2m_al_sec_obj_param_t;

typedef struct
{
    uint16_t            server_id;
    char               *binding;        /* support U and UQ bind mode */
    int                 life_time;      /* required option */
    uint32_t            storing_cnt;    /* storing count */
} lwm2m_al_srv_obj_param_t;

typedef int (*fn_lwm2m_msg_deal)(int type, char *msg, int len);

typedef struct
{
    char               *endpoint_name;
    fn_lwm2m_msg_deal   dealer;         /* command dealer, called to process iot platform */
    void               *user_data;
    int                 bootstrap_type;
} lwm2m_al_init_param_t;

typedef struct
{
    int                 object_id;
    int                 object_instance_id;
    int                 resource_id;
    void               *data;
    int                 length;
    int                 mode;           /* mode: 0-nonconfirmable；1-confirmable */
} lwm2m_al_send_param_t;

/* opertate interface */
typedef struct
{
    /* lwm2m config, prepare endpoint name and message deal callback */
    int (*config)(lwm2m_al_init_param_t *init_param);

    /* lwm2m deinit */
    int (*deconfig)(void);

    /* lwm2m add object */
    int (*add_object)(int object_id, int object_instance_id, uint16_t resource_id, void *param);

    /* lwm2m delete object */
    int (*delete_object)(int object_id);

    /* lwm2m connect */
    int (*connect)(void);

    /* lwm2m disconnect */
    int (*disconnect)(void);

    /* lwm2m send */
    int (*send)(lwm2m_al_send_param_t *send_param);

    /* lwm2m receive */
    int (*recv)(void);
} lwm2m_al_op_t;

/**
 * @brief     : the lwm2m service should call this function to init necessary parameters
 *
 *@param[in] init_param  refer to the data of lwm2m_al_init_param_t
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_config(lwm2m_al_init_param_t *init_param);

/**
 * @brief     : when you don't want to use the lwm2m service ,please call this function
 *
 *@param[in] void
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_deconfig(void);

/**
 * @brief     : the lwm2m service should call this function to add some necessary objects
 *
 *@param[in] op  refer to the data  lwm2m_al_op_t
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_add_object(int object_id, int object_instance_id, uint16_t resource_id, void *param);

/**
 * @brief     : you could call this function to delete the object by given object id
 *
 *@param[in] object_id
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_delete_object(int object_id);

/**
 * @brief     : you could use this function to connect to the lwm2m server
 *
 *@param[in] void
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_connect(void);

/**
 * @brief     : you could use this function to disconnect from the lwm2m server
 *
 *@param[in] void
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_disconnect(void);

/**
 * @brief     : you could use this function to send a message to the server
 *
 *@param[in] send_param refer to the data of lwm2m_al_send_param_t
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_send(lwm2m_al_send_param_t *send_param);

/**
 * @brief     : you could use this function receive data from the server
 *
 *@param[in] void
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_receive(void);

/**
 * @brief     : the lwm2m lib should call this function to register its implement as a service
 *
 *@param[in] op  refer to the data of lwm2m_al_op_t
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_install(lwm2m_al_op_t *op);

/**
 * @brief     : call this function to unregister the lwm2m service
 *
 *@param[in] void
 *
 * @return 0 success,otherwise it's a special error code if failed
 */
int lwm2m_al_uninstall(void);

#endif /* LITEOS_LAB_IOT_LINK_NETWORK_LWM2M_LWM2M_AL_LWM2M_AL_H_ */

