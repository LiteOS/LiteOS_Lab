/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2019>, <Huawei Technologies Co., Ltd>
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

#include "internals.h"


bool lwm2m_isBoostrpEnable(const lwm2m_context_t *contextP)
{
#ifdef LWM2M_BOOTSTRAP
    return contextP->bsCtrl.bsType != BOOTSTRAP_FACTORY;
#else
    return false;
#endif
}

void lwm2m_initBsCtrlStat(lwm2m_context_t *contextP, lwm2m_bootstrap_type_e bs_type)
{
    memset(&contextP->bsCtrl, 0, sizeof(contextP->bsCtrl));
    contextP->bsCtrl.bsType = bs_type;
    contextP->bsCtrl.state = STATE_INITIAL;
}

static void lwm2m_setBsCtrlStatWithoutCheck(lwm2m_context_t *contextP, lwm2m_client_state_t state)
{
    if (contextP->bsCtrl.state != state)
    {
        contextP->bsCtrl.state = state;
        contextP->bsCtrl.cnt = 0;
        return;
    }
    {
        uint32_t maxValue = ((STATE_REGISTER_REQUIRED == state)
                            ? MAX_FACTORY_BS_RETRY_CNT : MAX_CLIENT_INITIATED_BS_RETRY_CNT);

        if(++(contextP->bsCtrl.cnt) >= maxValue)
        {
            contextP->bsCtrl.state = ((STATE_REGISTER_REQUIRED == contextP->bsCtrl.state)
                                     ? STATE_BOOTSTRAP_REQUIRED :STATE_REGISTER_REQUIRED);
            contextP->bsCtrl.cnt = 0;
        }
    }

}

void lwm2m_setBsCtrlStat(lwm2m_context_t *contextP, lwm2m_client_state_t state)
{
    lwm2m_client_state_t oldState = contextP->bsCtrl.state;
    uint32_t oldCnt = contextP->bsCtrl.cnt;

    lwm2m_setBsCtrlStatWithoutCheck(contextP, state);

    if (STATE_REGISTER_REQUIRED == contextP->bsCtrl.state)
    {
        if (contextP->serverList == NULL)
        {
            contextP->bsCtrl.state = STATE_BOOTSTRAP_REQUIRED;
            contextP->bsCtrl.cnt = 0;
        }
    }

    if (STATE_BOOTSTRAP_REQUIRED == contextP->bsCtrl.state)
    {
        if ((!lwm2m_isBoostrpEnable(contextP))
            || (contextP->bootstrapServerList == NULL))
        {
            contextP->bsCtrl.state = STATE_REGISTER_REQUIRED;
            contextP->bsCtrl.cnt = 0;
            goto END;
        }

        // bootstrapServerList not empty, int CIBS but bs ip not valid.
#if defined(LWM2M_BOOTSTRAP)
        if (!bootstrap_isBsServerIpValid(contextP))
#endif
        {
            //FBS
            if (contextP->serverList)
            {
                contextP->bsCtrl.state = STATE_REGISTER_REQUIRED;
                contextP->bsCtrl.cnt = 0;
            }
        }
    }
END:
    if ((oldState != contextP->bsCtrl.state)
        || (oldCnt != contextP->bsCtrl.cnt))
    {
        LOG_ARG("bsctrlstat (%d,%d) to (%d,%d)", oldState, oldCnt,
                    contextP->bsCtrl.state, contextP->bsCtrl.cnt);
    }

}

void lwm2m_delayBsRetry(lwm2m_context_t *contextP)
{
    uint32_t delayBase;
    uint32_t delayInterval;
    uint32_t cnt = contextP->bsCtrl.cnt;
    uint32_t expireTime;

    if (contextP->bsCtrl.state == STATE_REGISTER_REQUIRED)
    {
        delayBase = FACTORY_BS_DELAY_BASE;
        delayInterval = FACTORY_BS_DELAY_INTERVAL;
    }
    else
    {
        delayBase = CLIENT_INITIATED_BS_DELAY_BASE;
        delayInterval = CLIENT_INITIATED_BS_DELAY_INTERVAL;
    }

    expireTime = delayBase + delayInterval * cnt;
    if (expireTime > 0)
    {
        lwm2m_delay(expireTime);
    }
}

lwm2m_client_state_t lwm2m_getBsCtrlStat(const lwm2m_context_t *contextP)
{
    return contextP->bsCtrl.state;
}

#ifdef LWM2M_CLIENT_MODE

static lwm2m_event_handler_t event_handler = NULL;
void lwm2m_register_event_handler(lwm2m_event_handler_t callback)
{
    event_handler = callback;
}

void lwm2m_notify_even(module_type_t type, int code, const char *arg, int arg_len)
{
    if(event_handler != NULL)
    {
        event_handler(type, code, arg, arg_len);
    }
}

void lwm2m_reset_register(lwm2m_context_t *context)
{
    lwm2m_server_t *server = context->serverList;

    LOG("Entering");
    while (NULL != server)
    {
        registration_reset(context, server);
        server = server->next;
    }
}

int lwm2m_initBootStrap(lwm2m_context_t *contextP, lwm2m_bootstrap_type_e bsType)
{
    if (NULL == contextP)
    {
        LOG("context null point");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    contextP->regist_first_flag = ((bsType == BOOTSTRAP_CLIENT_INITIATED) ? false : true);
    lwm2m_initBsCtrlStat(contextP, bsType);
    return COAP_NO_ERROR;
}

#endif

