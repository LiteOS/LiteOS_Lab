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
 * Copyright (c) 2013, 2014 Intel Corporation and others.
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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Julien Vermillard, Sierra Wireless
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *
 *******************************************************************************/

/*
 *  Resources:
 *
 *          Name         | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *  Short ID             |  0 |     R      |  Single   |    Yes    | Integer | 1-65535 |       |
 *  Lifetime             |  1 |    R/W     |  Single   |    Yes    | Integer |         |   s   |
 *  Default Min Period   |  2 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Default Max Period   |  3 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Disable              |  4 |     E      |  Single   |    No     |         |         |       |
 *  Disable Timeout      |  5 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Notification Storing |  6 |    R/W     |  Single   |    Yes    | Boolean |         |       |
 *  Binding              |  7 |    R/W     |  Single   |    Yes    | String  |         |       |
 *  Registration Update  |  8 |     E      |  Single   |    Yes    |         |         |       |
 *
 */

#include "internals.h"
#include "object_comm.h"

typedef struct _server_instance_
{
    struct _server_instance_   *next;    // matches lwm2m_list_t::next
    uint16_t                    instanceId;            // matches lwm2m_list_t::id
    uint16_t                    shortServerId;
    uint32_t                    lifetime;
    uint32_t                    defaultMinPeriod;
    uint32_t                    defaultMaxPeriod;
    uint32_t                    disableTimeout;
    bool                        storing;
    char                        binding[4];
} server_instance_t;

static uint8_t prv_get_value(lwm2m_data_t *dataP,
                             server_instance_t *targetP)
{
    switch (dataP->id)
    {
        case LWM2M_SERVER_SHORT_ID_ID:
            lwm2m_data_encode_int(targetP->shortServerId, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_LIFETIME_ID:
            lwm2m_data_encode_int(targetP->lifetime, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_MIN_PERIOD_ID:
            lwm2m_data_encode_int(targetP->defaultMinPeriod, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_MAX_PERIOD_ID:
            lwm2m_data_encode_int(targetP->defaultMaxPeriod, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_DISABLE_ID:
            return COAP_405_METHOD_NOT_ALLOWED;

        case LWM2M_SERVER_TIMEOUT_ID:
            lwm2m_data_encode_int(targetP->disableTimeout, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_STORING_ID:
            lwm2m_data_encode_bool(targetP->storing, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_BINDING_ID:
            lwm2m_data_encode_string(targetP->binding, dataP);
            return COAP_205_CONTENT;

        case LWM2M_SERVER_UPDATE_ID:
            return COAP_405_METHOD_NOT_ALLOWED;

        default:
            return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_server_read(uint16_t instanceId,
                               int *numDataP,
                               lwm2m_data_t **dataArrayP,
                               lwm2m_data_cfg_t *dataCfg,
                               lwm2m_object_t *objectP)
{
    server_instance_t *targetP;
    uint8_t result;
    int i;
    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full instance ?
    if (0 == *numDataP)
    {
        uint16_t resList[] =
        {
            LWM2M_SERVER_SHORT_ID_ID,
            LWM2M_SERVER_LIFETIME_ID,
            LWM2M_SERVER_MIN_PERIOD_ID,
            LWM2M_SERVER_MAX_PERIOD_ID,
            LWM2M_SERVER_TIMEOUT_ID,
            LWM2M_SERVER_STORING_ID,
            LWM2M_SERVER_BINDING_ID
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);
        *dataArrayP = lwm2m_data_new(nbRes);

        if (NULL == *dataArrayP) return COAP_500_INTERNAL_SERVER_ERROR;

        *numDataP = nbRes;

        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;

    do
    {
        result = prv_get_value((*dataArrayP) + i, targetP);
        i++;
    }
    while ((i < *numDataP) && (COAP_205_CONTENT == result));

    return result;
}

static uint8_t prv_server_discover(uint16_t instanceId,
                                   int *numDataP,
                                   lwm2m_data_t **dataArrayP,
                                   lwm2m_object_t *objectP)
{
    uint8_t result;
    int i;
    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (0 == *numDataP)
    {
        uint16_t resList[] =
        {
            LWM2M_SERVER_SHORT_ID_ID,
            LWM2M_SERVER_LIFETIME_ID,
            LWM2M_SERVER_MIN_PERIOD_ID,
            LWM2M_SERVER_MAX_PERIOD_ID,
            LWM2M_SERVER_DISABLE_ID,
            LWM2M_SERVER_TIMEOUT_ID,
            LWM2M_SERVER_STORING_ID,
            LWM2M_SERVER_BINDING_ID,
            LWM2M_SERVER_UPDATE_ID
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);
        *dataArrayP = lwm2m_data_new(nbRes);

        if (NULL == *dataArrayP) return COAP_500_INTERNAL_SERVER_ERROR;

        *numDataP = nbRes;

        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; (i < *numDataP) && (COAP_205_CONTENT == result); i++)
        {
            switch ((*dataArrayP)[i].id)
            {
                case LWM2M_SERVER_SHORT_ID_ID:
                case LWM2M_SERVER_LIFETIME_ID:
                case LWM2M_SERVER_MIN_PERIOD_ID:
                case LWM2M_SERVER_MAX_PERIOD_ID:
                case LWM2M_SERVER_DISABLE_ID:
                case LWM2M_SERVER_TIMEOUT_ID:
                case LWM2M_SERVER_STORING_ID:
                case LWM2M_SERVER_BINDING_ID:
                case LWM2M_SERVER_UPDATE_ID:
                    break;

                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_set_int_value(lwm2m_data_t *dataArray, uint32_t *data)
{
    uint8_t result;
    int64_t value;

    if (1 == lwm2m_data_decode_int(dataArray, &value))
    {
        if (value >= 0 && value <= 0xFFFFFFFF)
        {
            *data = value;
            result = COAP_204_CHANGED;
        }
        else
        {
            result = COAP_406_NOT_ACCEPTABLE;
        }
    }
    else
    {
        result = COAP_400_BAD_REQUEST;
    }

    return result;
}

bool prv_server_check_binding_valid(const char *binding, size_t size)
{
    if ((NULL != binding) && (strncmp(binding, "U", size) == 0
                              || strncmp(binding, "UQ", size) == 0
                              || strncmp(binding, "S", size) == 0
                              || strncmp(binding, "SQ", size) == 0
                              || strncmp(binding, "US", size) == 0
                              || strncmp(binding, "UQS", size) == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}
static uint8_t prv_server_write(uint16_t instanceId,
                                int numData,
                                lwm2m_data_t *dataArray,
                                lwm2m_object_t *objectP)
{
    char uri[MAX_STRURI_LEN] = {0};
    server_instance_t *targetP;
    int i;
    uint8_t result;
    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

    if (NULL == targetP)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;

    do
    {
        switch (dataArray[i].id)
        {
            case LWM2M_SERVER_SHORT_ID_ID:
            {
                uint32_t value = targetP->shortServerId;
                result = prv_set_int_value(dataArray + i, &value);

                if (COAP_204_CHANGED == result)
                {
                    if (0 < value && 0xFFFF >= value)
                    {
                        targetP->shortServerId = value;
                    }
                    else
                    {
                        result = COAP_406_NOT_ACCEPTABLE;
                    }
                }
            }
            break;

            case LWM2M_SERVER_LIFETIME_ID:
                result = prv_set_int_value(dataArray + i, (uint32_t *) & (targetP->lifetime));
                break;

            case LWM2M_SERVER_MIN_PERIOD_ID:
                result = prv_set_int_value(dataArray + i, &(targetP->defaultMinPeriod));
                break;

            case LWM2M_SERVER_MAX_PERIOD_ID:
                result = prv_set_int_value(dataArray + i, &(targetP->defaultMaxPeriod));
                break;

            case LWM2M_SERVER_DISABLE_ID:
                result = COAP_405_METHOD_NOT_ALLOWED;
                break;

            case LWM2M_SERVER_TIMEOUT_ID:
                result = prv_set_int_value(dataArray + i, &(targetP->disableTimeout));
                break;

            case LWM2M_SERVER_STORING_ID:
            {
                bool value;

                if (1 == lwm2m_data_decode_bool(dataArray + i, &value))
                {
                    targetP->storing = value;

                    if (!value)
                    {
                        set_binary_app_data_object_rpt_max_cnt(value);
                    }

                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_400_BAD_REQUEST;
                }
            }
            break;

            case LWM2M_SERVER_BINDING_ID:
                if (((LWM2M_TYPE_STRING == dataArray[i].type) || (LWM2M_TYPE_OPAQUE == dataArray[i].type))
                    && (dataArray[i].value.asBuffer.length > 0) && (dataArray[i].value.asBuffer.length <= 3)
                    && prv_server_check_binding_valid((char *)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length))
                {
                    (void) memset(targetP->binding, 0, sizeof(targetP->binding));
                    (void) strncpy(targetP->binding, (char *)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
                    result = COAP_204_CHANGED;
                }
                else
                {
                    result = COAP_400_BAD_REQUEST;
                }

                break;

            case LWM2M_SERVER_UPDATE_ID:
                result = COAP_405_METHOD_NOT_ALLOWED;
                break;

            case LWM2M_SERVER_BSTRIGGER:
                snprintf(uri, MAX_STRURI_LEN, URI_FMT, objectP->objID, instanceId, dataArray[i].id);
                lwm2m_cmd_ioctl(LWM2M_CMD_TRIGER_SERVER_INITIATED_BS, NULL, 0, LWM2M_AL_OP_SERVERREBS, uri);
                result = COAP_204_CHANGED;
                break;

            default:
                return COAP_404_NOT_FOUND;
        }

        i++;
    }
    while ((i < numData) && (COAP_204_CHANGED == result));

    return result;
}

static uint8_t prv_server_execute(uint16_t instanceId,
                                  uint16_t resourceId,
                                  uint8_t *buffer,
                                  int length,
                                  lwm2m_object_t *objectP)

{
    char uri[MAX_STRURI_LEN] = {0};
    server_instance_t *targetP;
    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

    if (NULL == targetP) return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
        case LWM2M_SERVER_DISABLE_ID:

            // executed in core, if COAP_204_CHANGED is returned
            if (0 < targetP->disableTimeout) return COAP_204_CHANGED;
            else return COAP_405_METHOD_NOT_ALLOWED;

        case LWM2M_SERVER_UPDATE_ID:
            // executed in core, if COAP_204_CHANGED is returned
            return COAP_204_CHANGED;

        case LWM2M_SERVER_BSTRIGGER:
            snprintf(uri, MAX_STRURI_LEN, URI_FMT, objectP->objID, instanceId, resourceId);
            lwm2m_cmd_ioctl(LWM2M_CMD_TRIGER_SERVER_INITIATED_BS, NULL, 0, LWM2M_AL_OP_SERVERREBS, uri);
            return COAP_204_CHANGED;
            break;

        default:
            return COAP_405_METHOD_NOT_ALLOWED;
    }
}

static uint8_t prv_server_delete(uint16_t id,
                                 lwm2m_object_t *objectP)
{
    server_instance_t *serverInstance;
    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&serverInstance);

    if (NULL == serverInstance) return COAP_404_NOT_FOUND;

    lwm2m_free(serverInstance);
    return COAP_202_DELETED;
}

static uint8_t prv_server_create(uint16_t instanceId,
                                 int numData,
                                 lwm2m_data_t *dataArray,
                                 lwm2m_object_t *objectP)
{
    server_instance_t *serverInstance;
    uint8_t result;
    serverInstance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));

    if (NULL == serverInstance) return COAP_500_INTERNAL_SERVER_ERROR;

    (void) memset(serverInstance, 0, sizeof(server_instance_t));
    serverInstance->instanceId = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, serverInstance);
    result = prv_server_write(instanceId, numData, dataArray, objectP);

    if (COAP_204_CHANGED != result)
    {
        (void)prv_server_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

void copy_server_object(lwm2m_object_t *objectDest, lwm2m_object_t *objectSrc)
{
    server_instance_t *instanceSrc = NULL;
    server_instance_t *previousInstanceDest = NULL;
    (void) memcpy(objectDest, objectSrc, sizeof(lwm2m_object_t));
    objectDest->instanceList = NULL;
    objectDest->userData = NULL;
    instanceSrc = (server_instance_t *)objectSrc->instanceList;

    while (NULL != instanceSrc)
    {
        server_instance_t *instanceDest = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));

        if (NULL == instanceDest)
        {
            return;
        }

        (void) memcpy(instanceDest, instanceSrc, sizeof(server_instance_t));
        // not sure it's necessary:
        (void) strncpy(instanceDest->binding, instanceSrc->binding, 4);
        instanceSrc = (server_instance_t *)instanceSrc->next;

        if (NULL == previousInstanceDest)
        {
            objectDest->instanceList = (lwm2m_list_t *)instanceDest;
        }
        else
        {
            previousInstanceDest->next = instanceDest;
        }

        previousInstanceDest = instanceDest;
    }
}

void display_server_object(lwm2m_object_t *object)
{
#ifdef WITH_LOGS
    fprintf(stdout, "  /%u: Server object, instances:\r\n", object->objID);
    server_instance_t *serverInstance = (server_instance_t *)object->instanceList;

    while (NULL != serverInstance)
    {
        fprintf(stdout, "    /%u/%u: instanceId: %u, shortServerId: %u, lifetime: %u, storing: %s, binding: %s\r\n",
                object->objID, serverInstance->instanceId,
                serverInstance->instanceId, serverInstance->shortServerId, serverInstance->lifetime,
                serverInstance->storing ? "true" : "false", serverInstance->binding);
        serverInstance = (server_instance_t *)serverInstance->next;
    }

#endif
}

int add_server_object_instance(lwm2m_object_t *obj, int object_instance_id, void *param)
{
    server_instance_t *instance = NULL;
    lwm2m_al_srv_obj_param_t *srv_param = (lwm2m_al_srv_obj_param_t *)param;

    if ((NULL == obj) || (NULL == param))
    {
        return LWM2M_ARG_INVALID;
    }

    // Manually create an hardcoded server
    instance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));

    if (NULL == instance)
    {
        return LWM2M_MALLOC_FAILED;
    }

    (void) memset(instance, 0, sizeof(server_instance_t));
    instance->instanceId = 0;
    instance->shortServerId = srv_param->server_id;

    if (srv_param->life_time <= 0)
    {
        instance->lifetime = LWM2M_DEFAULT_LIFETIME;
    }
    else
    {
        instance->lifetime = srv_param->life_time;
    }

    instance->storing = (srv_param->storing_cnt != 0);

    if (prv_server_check_binding_valid(srv_param->binding, strlen(srv_param->binding)))
    {
        (void) memcpy(instance->binding, srv_param->binding, strlen(srv_param->binding) + 1);
    }
    else
    {
        lwm2m_free(instance);
        return LWM2M_MALLOC_FAILED;
    }

    obj->instanceList = LWM2M_LIST_ADD(obj->instanceList, instance);
    return LWM2M_OK;
}

int config_server_object(lwm2m_object_t *obj, void *param)
{
    if (NULL == obj)
    {
        return LWM2M_ARG_INVALID;
    }

    obj->readFunc = prv_server_read;
    obj->discoverFunc = prv_server_discover;
    obj->writeFunc = prv_server_write;
    obj->executeFunc = prv_server_execute;
    obj->createFunc = prv_server_create;
    obj->deleteFunc = prv_server_delete;
    return LWM2M_OK;
}

void clean_server_object(lwm2m_object_t *object)
{
    if (NULL == object)
    {
        return;
    }

    while (NULL != object->instanceList)
    {
        server_instance_t *serverInstance = (server_instance_t *)object->instanceList;
        object->instanceList = object->instanceList->next;
        lwm2m_free(serverInstance);
    }

    lwm2m_free(object);
}
