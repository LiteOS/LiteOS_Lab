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

struct crypto_acc {
    const char        * name;
    void              * methods;
    void              * pdata;
    struct crypto_acc * next;
};

static struct crypto_acc * __crypto_acc_list = NULL;

int crypto_acc_register (const char * name, void * methods, void * pdata)
{
    int                 flags;
    struct crypto_acc * acc;

    if (name == NULL || methods == NULL) {
        return -1;
    }

    acc = malloc (sizeof (struct crypto_acc));

    acc->methods = methods;
    acc->pdata   = pdata;
    acc->name    = name;

    if (acc == NULL) {
        return -1;
    }

    if (!osal_int_lock (&flags)) {
        free (acc);
        return -1;
    }

    acc->next         = __crypto_acc_list;

    __crypto_acc_list = acc;

    osal_int_restore (flags);

    return 0;
}

int crypto_acc_get (const char * name, void ** methods, void ** pdata)
{
    int                 flags;
    struct crypto_acc * acc;
    int                 ret = -1;

    if (name == NULL || methods == NULL || pdata == NULL) {
        return -1;
    }

    if (!osal_int_lock (&flags)) {
        return -1;
    }

    acc = __crypto_acc_list;

    while (acc != NULL) {
        if (strcmp (acc->name, name) == 0) {
            ret = 0;
            *methods = acc->methods;
            *pdata   = acc->pdata;
            break;
        }
    
        acc = acc->next;
    }

    osal_int_restore (flags);

    return ret;
}