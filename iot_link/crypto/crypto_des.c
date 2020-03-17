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
#include <crypto_des.h>

static struct crypto_des_methods * __des_acc_methods;
static void                      * __des_acc_pdata;

static int __des_acc_init (void)
{
    return crypto_acc_get ("des", (void **) &__des_acc_methods, &__des_acc_pdata);
}

static int __crypto_acc_des (const uint8_t * in, uint8_t * out, size_t len,
                             const uint8_t * key, bool_t encrypt)
{
    if (in == NULL || out == NULL || len == 0 || key == NULL) {
        return -1;
    }

    if (__des_acc_methods == NULL && __des_acc_init () != 0) {
        return -1;
    }

    if (__des_acc_methods->encrypt == NULL || __des_acc_methods->decrypt == NULL) {
        return -1;
    }

    return encrypt ? __des_acc_methods->encrypt (__des_acc_pdata, in, out, len, key)
                   : __des_acc_methods->decrypt (__des_acc_pdata, in, out, len, key);
}

/**
 * crypto_acc_des_encrypt - des buffer encryption
 * @in:  the input buffer to be encrypted
 * @out: the output encrypted buffer
 * @len: the length of the buffer
 * @key: the encrypt key
 *
 * return: 0 on success, negtive value on error
 */

int crypto_acc_des_encrypt (const uint8_t * in, uint8_t * out, size_t len, const uint8_t * key)
{
    return __crypto_acc_des (in, out, len, key, true);
}

/**
 * crypto_acc_des_decrypt - des buffer decryption
 * @in:  the input buffer to be decrypted
 * @out: the output decrypted buffer
 * @len: the length of the buffer
 * @key: the decrypt key
 *
 * return: 0 on success, negtive value on error
 */

int crypto_acc_des_decrypt (const uint8_t * in, uint8_t * out, size_t len, const uint8_t * key)
{
    return __crypto_acc_des (in, out, len, key, false);
}

#if 1
#include <stdint.h>
#include <stddef.h>

static int __encrypt (void * pdata, const uint8_t * in, uint8_t * out, size_t len, const uint8_t * key)
{
    return 0;
}

static int __decrypt (void * pdata, const uint8_t * in, uint8_t * out, size_t len, const uint8_t * key)
{
    return 0;
}

static struct crypto_des_methods __fake_des_acc_methods = {
    __encrypt,
    __decrypt,
};

int des_acc_test (void)
{
    uint8_t result [32];
    uint8_t buff [128];

    crypto_acc_register ("des", &__fake_des_acc_methods, NULL);

    crypto_acc_des_encrypt (buff, buff, 128, result);
    crypto_acc_des_decrypt (buff, buff, 128, result);

    return 0;
}

#endif