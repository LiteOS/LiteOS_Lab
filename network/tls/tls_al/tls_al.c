/* ----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
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
 * --------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 * --------------------------------------------------------------------------- */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "tls_al.h"
#include "link_log.h"

static const tls_al_t *g_tls_al;

en_tls_al_err_t tls_al_new(tls_al_para_t *para, void **handle)
{
    en_tls_al_err_t ret = EN_TLS_AL_ERR_NOCONFIG;
    if ((NULL != g_tls_al) && (NULL != g_tls_al->io.io_new)) {
        ret = g_tls_al->io.io_new(para, handle);
    }
    return ret;
}

int tls_al_connect(void *handle, const char *server_ip, const char *server_port, int timeout)
{
    int ret = -1;
    if ((NULL != g_tls_al) && (NULL != g_tls_al->io.io_connect)) {
        ret = g_tls_al->io.io_connect(handle, server_ip, server_port, timeout);
    }
    return ret;
}

int tls_al_write(void *handle, uint8_t *msg, size_t len, int timeout)
{
    int ret = 0;
    if ((NULL != g_tls_al) && (NULL != g_tls_al->io.io_write)) {
        ret = g_tls_al->io.io_write(handle, msg, len, timeout);
    }
    return ret;
}

int tls_al_read(void *handle, uint8_t *buf, size_t len, int timeout)
{
    int ret = 0;
    if ((NULL != g_tls_al) && (NULL != g_tls_al->io.io_read)) {
        ret = g_tls_al->io.io_read(handle, buf, len, timeout);
    }
    return ret;
}

en_tls_al_err_t tls_al_destroy(void *handle)
{
    en_tls_al_err_t ret = EN_TLS_AL_ERR_NOCONFIG;
    if ((NULL != g_tls_al) && (NULL != g_tls_al->io.io_destroy)) {
        ret = g_tls_al->io.io_destroy(handle);
    }
    return ret;
}

int tls_al_install(const tls_al_t *tls)
{
    int ret = -1;
    if (NULL == g_tls_al) {
        g_tls_al = tls;
        ret = 0;
    }

    return ret;
}

int tls_al_uninstall(const char *name)
{
    int ret = -1;

    if ((NULL != g_tls_al) && (0 == strcmp(name, g_tls_al->name))) {
        g_tls_al = NULL;
    }

    return ret;
}

__attribute__((weak)) int tls_imp_init(void)
{
    LINK_LOG_DEBUG("%s:###please implement tls by yourself####", __FUNCTION__);
    return -1;
}

int tls_al_init(void)
{
    int ret;

    ret = tls_imp_init();
    LINK_LOG_DEBUG("IOT_LINK:DO TLS LOAD-IMPLEMENT RET:%d", ret);

    return ret;
}
