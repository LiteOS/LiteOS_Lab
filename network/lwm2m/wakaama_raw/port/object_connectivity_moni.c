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

/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH Germany.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *
 *******************************************************************************/

/*
 *  This Connectivity Monitoring object is optional and has a single instance
 *
 *  Resources:
 *
 *          Name             | ID | Oper. | Inst. | Mand.|  Type   | Range | Units |
 *  Network Bearer           |  0 |  R    | Single|  Yes | Integer |       |       |
 *  Available Network Bearer |  1 |  R    | Multi |  Yes | Integer |       |       |
 *  Radio Signal Strength    |  2 |  R    | Single|  Yes | Integer |       | dBm   |
 *  Link Quality             |  3 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  IP Addresses             |  4 |  R    | Multi |  Yes | String  |       |       |
 *  Router IP Addresses      |  5 |  R    | Multi |  No  | String  |       |       |
 *  Link Utilization         |  6 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  APN                      |  7 |  R    | Multi |  No  | String  |       |       |
 *  Cell ID                  |  8 |  R    | Single|  No  | Integer |       |       |
 *  SMNC                     |  9 |  R    | Single|  No  | Integer | 0-999 |   %   |
 *  SMCC                     | 10 |  R    | Single|  No  | Integer | 0-999 |       |
 *
 */

#include "internals.h"
#include <lwm2m_common.h>

// Resource Id's:
#define RES_M_NETWORK_BEARER            0
#define RES_M_AVL_NETWORK_BEARER        1
#define RES_M_RADIO_SIGNAL_STRENGTH     2
#define RES_O_LINK_QUALITY              3
#define RES_M_IP_ADDRESSES              4
#define RES_O_ROUTER_IP_ADDRESS         5
#define RES_O_LINK_UTILIZATION          6
#define RES_O_APN                       7
#define RES_O_CELL_ID                   8
#define RES_O_SMNC                      9
#define RES_O_SMCC                      10

#define VALUE_NETWORK_BEARER_GSM        0   //GSM see
#define VALUE_AVL_NETWORK_BEARER_1      0   //GSM
#define VALUE_AVL_NETWORK_BEARER_2      21  //WLAN
#define VALUE_AVL_NETWORK_BEARER_3      41  //Ethernet
#define VALUE_AVL_NETWORK_BEARER_4      42  //DSL
#define VALUE_AVL_NETWORK_BEARER_5      43  //PLC
#define VALUE_IP_ADDRESS_1              "192.168.178.101"
#define VALUE_IP_ADDRESS_2              "192.168.178.102"
#define VALUE_ROUTER_IP_ADDRESS_1       "192.168.178.001"
#define VALUE_ROUTER_IP_ADDRESS_2       "192.168.178.002"
#define VALUE_APN_1                     "web.vodafone.de"
#define VALUE_APN_2                     "cda.vodafone.de"
#define VALUE_CELL_ID                   69696969
#define VALUE_RADIO_SIGNAL_STRENGTH     80                  //dBm
#define VALUE_LINK_QUALITY              98
#define VALUE_LINK_UTILIZATION          666
#define VALUE_SMNC                      33
#define VALUE_SMCC                      44
#define IP4ADDR_STRLEN_MAX              16
#define IPADDRT_LIMIT 2
typedef struct
{
    char    ipAddresses[IPADDRT_LIMIT][IP4ADDR_STRLEN_MAX];        // limited to 2!
    char    routerIpAddresses[IPADDRT_LIMIT][IP4ADDR_STRLEN_MAX];  // limited to 2!
    long    cellId;
    int     signalStrength;
    int     linkQuality;
    int     linkUtilization;
} conn_m_data_t;

static uint8_t prv_set_value(lwm2m_data_t *dataP,
                             conn_m_data_t *connDataP)
{
    switch (dataP->id)
    {
        case RES_M_NETWORK_BEARER:
            lwm2m_data_encode_int(VALUE_NETWORK_BEARER_GSM, dataP);
            return COAP_205_CONTENT;

        case RES_M_AVL_NETWORK_BEARER:
        {
            int riCnt = 1;   // reduced to 1 instance to fit in one block size
            lwm2m_data_t *subTlvP;
            int networkbearer = 0;
            subTlvP = lwm2m_data_new(riCnt);

            if (NULL == subTlvP) return COAP_500_INTERNAL_SERVER_ERROR;

            subTlvP[0].id    = 0;
            (void)lwm2m_cmd_ioctl(LWM2M_GET_NETWORK_BEARER, (char *)&networkbearer, sizeof(int));
            lwm2m_data_encode_int(networkbearer, subTlvP);
            lwm2m_data_encode_instances(subTlvP, riCnt, dataP);
            return COAP_205_CONTENT ;
        }

        case RES_M_RADIO_SIGNAL_STRENGTH: //s-int
        {
            int signalstrength = 0;
            (void)lwm2m_cmd_ioctl(LWM2M_GET_SIGNAL_STRENGTH, (char *)&signalstrength, sizeof(int));
            lwm2m_data_encode_int(connDataP->signalStrength, dataP);
            return COAP_205_CONTENT;
        }

        case RES_O_LINK_QUALITY: //s-int
        {
            int linkQuality;
            (void)lwm2m_cmd_ioctl(LWM2M_GET_LINK_QUALITY, (char *)&linkQuality, sizeof(int));
            lwm2m_data_encode_int(linkQuality, dataP);
            return COAP_205_CONTENT ;
        }

        case RES_M_IP_ADDRESSES:
        {
            int ri, riCnt = 1;   // reduced to 1 instance to fit in one block size
            lwm2m_data_t *subTlvP = lwm2m_data_new(riCnt);

            if (NULL == subTlvP) return COAP_500_INTERNAL_SERVER_ERROR;

            for (ri = 0; ri < riCnt; ri++)
            {
                subTlvP[ri].id = ri;
                lwm2m_data_encode_string(connDataP->ipAddresses[ri], subTlvP + ri);
            }

            lwm2m_data_encode_instances(subTlvP, riCnt, dataP);
            return COAP_205_CONTENT ;
        }

        case RES_O_ROUTER_IP_ADDRESS:
        {
            int ri, riCnt = 1;   // reduced to 1 instance to fit in one block size
            lwm2m_data_t *subTlvP = lwm2m_data_new(riCnt);

            if (NULL == subTlvP) return COAP_500_INTERNAL_SERVER_ERROR;

            for (ri = 0; ri < riCnt; ri++)
            {
                subTlvP[ri].id = ri;
                lwm2m_data_encode_string(connDataP->routerIpAddresses[ri], subTlvP + ri);
            }

            lwm2m_data_encode_instances(subTlvP, riCnt, dataP);
            return COAP_205_CONTENT ;
        }

        case RES_O_LINK_UTILIZATION:
        {
            int linkUtilization;
            (void)lwm2m_cmd_ioctl(LWM2M_GET_LINK_UTILIZATION, (char *)&linkUtilization, sizeof(int));
            lwm2m_data_encode_int(connDataP->linkUtilization, dataP);
            return COAP_205_CONTENT;
        }

        case RES_O_APN:
        {
            int riCnt = 1;   // reduced to 1 instance to fit in one block size
            lwm2m_data_t *subTlvP;
            subTlvP = lwm2m_data_new(riCnt);

            if (NULL == subTlvP) return COAP_500_INTERNAL_SERVER_ERROR;

            subTlvP[0].id     = 0;
            lwm2m_data_encode_string(VALUE_APN_1, subTlvP);
            lwm2m_data_encode_instances(subTlvP, riCnt, dataP);
            return COAP_205_CONTENT;
        }

        case RES_O_CELL_ID:
        {
            int cellId = 0;
            (void)lwm2m_cmd_ioctl(LWM2M_GET_CELL_ID, (char *)&cellId, sizeof(int));
            lwm2m_data_encode_int(cellId, dataP);
            return COAP_205_CONTENT ;
        }

        case RES_O_SMNC:
            lwm2m_data_encode_int(VALUE_SMNC, dataP);
            return COAP_205_CONTENT ;

        case RES_O_SMCC:
            lwm2m_data_encode_int(VALUE_SMCC, dataP);
            return COAP_205_CONTENT ;

        default:
            return COAP_404_NOT_FOUND ;
    }
}

static uint8_t prv_read(uint16_t instanceId,
                        int *numDataP,
                        lwm2m_data_t **dataArrayP,
                        lwm2m_data_cfg_t *dataCfg,
                        lwm2m_object_t *objectP)
{
    uint8_t result;
    int i;

    // this is a single instance object
    if (0 != instanceId)
    {
        return COAP_404_NOT_FOUND ;
    }

    // is the server asking for the full object ?
    if (0 == *numDataP)
    {
        uint16_t resList[] =
        {
            RES_M_NETWORK_BEARER,
            RES_M_AVL_NETWORK_BEARER,
            RES_M_RADIO_SIGNAL_STRENGTH,
            RES_O_LINK_QUALITY,
            RES_M_IP_ADDRESSES,
            RES_O_ROUTER_IP_ADDRESS,
            RES_O_LINK_UTILIZATION,
            RES_O_APN,
            RES_O_CELL_ID,
            RES_O_SMNC,
            RES_O_SMCC
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);
        *dataArrayP = lwm2m_data_new(nbRes);

        if (NULL == *dataArrayP)
            return COAP_500_INTERNAL_SERVER_ERROR ;

        *numDataP = nbRes;

        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;

    do
    {
        result = prv_set_value((*dataArrayP) + i, (conn_m_data_t *)(objectP->userData));
        i++;
    }
    while ((i < *numDataP) && (COAP_205_CONTENT == result));
           return result;
}

int add_connectivity_monitoring_object_instance(lwm2m_object_t *obj, int object_instance_id)
{
    if (NULL == obj)
    {
        return LWM2M_ARG_INVALID;
    }

    /*
     * and its unique instance
     *
     */
    obj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));

    if (NULL == obj->instanceList)
    {
        return LWM2M_MALLOC_FAILED;
    }

    (void) memset(obj->instanceList, 0, sizeof(lwm2m_list_t));
    obj->instanceList->id = object_instance_id;
    return LWM2M_OK;
}

int config_connectivity_monitoring_object(lwm2m_object_t *obj, void *param)
{
    if (NULL == obj)
    {
        return LWM2M_ARG_INVALID;
    }

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    obj->readFunc = prv_read;
    obj->executeFunc = NULL;
    obj->userData = lwm2m_malloc(sizeof(conn_m_data_t));

    /*
     * Also some user data can be stored in the object with a private structure containing the needed variables
     */
    if (NULL == obj->userData)
    {
        return LWM2M_MALLOC_FAILED;
    }

    conn_m_data_t *myData = (conn_m_data_t *) obj->userData;
    (void) memset((void *)myData, 0, sizeof(conn_m_data_t));
    myData->cellId          = VALUE_CELL_ID;
    myData->signalStrength  = VALUE_RADIO_SIGNAL_STRENGTH;
    myData->linkQuality     = VALUE_LINK_QUALITY;
    myData->linkUtilization = VALUE_LINK_UTILIZATION;
    (void) strcpy(myData->ipAddresses[0], VALUE_IP_ADDRESS_1);
    (void) strcpy(myData->ipAddresses[1], VALUE_IP_ADDRESS_2);
    (void) strcpy(myData->routerIpAddresses[0], VALUE_ROUTER_IP_ADDRESS_1);
    (void) strcpy(myData->routerIpAddresses[1], VALUE_ROUTER_IP_ADDRESS_2);
    return LWM2M_OK;
}

void free_object_conn_m(lwm2m_object_t *objectP)
{
    lwm2m_free(objectP->userData);
    lwm2m_list_free(objectP->instanceList);
    lwm2m_free(objectP);
}

uint8_t connectivity_moni_change(lwm2m_data_t *dataArray,
                                 lwm2m_object_t *objectP)
{
    int64_t value;
    uint8_t result;
    conn_m_data_t *data;
    data = (conn_m_data_t *)(objectP->userData);

    switch (dataArray->id)
    {
        case RES_M_RADIO_SIGNAL_STRENGTH:
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                data->signalStrength = value;
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }

            break;

        case RES_O_LINK_QUALITY:
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                data->linkQuality = value;
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }

            break;

        case RES_M_IP_ADDRESSES:
            if (sizeof(data->ipAddresses[0]) <= dataArray->value.asBuffer.length)
            {
                result = COAP_400_BAD_REQUEST;
            }
            else
            {
                (void) memset(data->ipAddresses[0], 0, sizeof(data->ipAddresses[0]));
                (void) memcpy(data->ipAddresses[0], dataArray->value.asBuffer.buffer, dataArray->value.asBuffer.length);
                data->ipAddresses[0][dataArray->value.asBuffer.length] = 0;
                result = COAP_204_CHANGED;
            }

            break;

        case RES_O_ROUTER_IP_ADDRESS:
            if (sizeof(data->routerIpAddresses[0]) <= dataArray->value.asBuffer.length)
            {
                result = COAP_400_BAD_REQUEST;
            }
            else
            {
                (void) memset(data->routerIpAddresses[0], 0, sizeof(data->routerIpAddresses[0]));
                (void) memcpy(data->routerIpAddresses[0], dataArray->value.asBuffer.buffer, dataArray->value.asBuffer.length);
                data->routerIpAddresses[0][dataArray->value.asBuffer.length] = 0;
                result = COAP_204_CHANGED;
            }

            break;

        case RES_O_CELL_ID:
            if (1 == lwm2m_data_decode_int(dataArray, &value))
            {
                data->cellId = value;
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }

            break;

        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
    }

    return result;
}

