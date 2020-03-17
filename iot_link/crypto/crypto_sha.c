/*----------------------------------------------------------------------------
 * Copyright (c) <2020>, <Huawei Technologies Co., Ltd>
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

#include <stdlib.h>
#include <string.h>

#include <osal.h>

#include <crypto.h>
#include <crypto_sha.h>

static struct crypto_sha_methods * __sha_acc_methods;
static void                      * __sha_acc_pdata;

static int __sha_acc_init (void)
{
    return crypto_acc_get ("sha", (void **) &__sha_acc_methods, &__sha_acc_pdata);
}

/**
 * crypto_acc_sha256 - calculate the sha256 for a buffer
 * @buff:   the buffer address
 * @len:    the buffer length
 * @result: address to hold the result
 *
 * return: 0 on success, negtive value on error
 */

int crypto_acc_sha256 (const uint8_t * buff, size_t len, uint8_t * result)
{
    if (buff == NULL || result == NULL) {
        return -1;
    }

    if (__sha_acc_methods == NULL && __sha_acc_init () != 0) {
        return -1;
    }

    if (__sha_acc_methods->sha256 == NULL) {
        return -1;
    }

    return __sha_acc_methods->sha256 (__sha_acc_pdata, buff, len, result);
}

/**
 * crypto_acc_sha512 - calculate the sha512 for a buffer
 * @buff:   the buffer address
 * @len:    the buffer length
 * @result: address to hold the result
 *
 * return: 0 on success, negtive value on error
 */

int crypto_acc_sha512 (const uint8_t * buff, size_t len, uint8_t * result)
{
    if (buff == NULL || result == NULL) {
        return -1;
    }

    if (__sha_acc_methods == NULL && __sha_acc_init () != 0) {
        return -1;
    }

    if (__sha_acc_methods->sha512 == NULL) {
        return -1;
    }

    return __sha_acc_methods->sha512 (__sha_acc_pdata, buff, len, result);
}

#if 1
#include <stdint.h>
#include <stddef.h>

static int __sha256 (void * pdata, const uint8_t * buff, size_t len, uint8_t * result)
{
    return 0;
}

static int __sha512 (void * pdata, const uint8_t * buff, size_t len, uint8_t * result)
{
    return 0;
}

static struct crypto_sha_methods __fake_sha_acc_methods = {
    __sha256,
    __sha512,
};

int sha_acc_test (void)
{
    uint16_t result [32];
    uint8_t  buff [128];

    crypto_acc_register ("sha", &__fake_sha_acc_methods, NULL);

    crypto_acc_sha256 (buff, 128, &result);
    crypto_acc_sha512 (buff, 128, &result);

    return 0;
}

#endif