/*----------------------------------------------------------------------------
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
 *  2020-06-08 19:17  zhangqianfu  The first version
 *
 */

#include "ota_sign.h"

#include <shell.h>
///< two way mqtt mode
static const char g_client_public[] = \
"-----BEGIN PUBLIC KEY-----\r\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA9HKy9M5zpGqZDSx0+dI+\r\n"
"CQSPfOTc7gB5ZQHBBqVGYQ0sTE6T6Rx22EoWZkJcQMKiUGLfuJBAsuSGAl7H8Isc\r\n"
"9DYXjuhciviZi/AdntC35VbaOhIlRw9GvSenCzqoVMiAv9A0cJEgg9jpiA8KYBlN\r\n"
"0vzSBci5VAWXyULHn1GYtHbbkGYlHt4DSceM9Itp32lfBXiKEEQ7fv6hzciV/cE1\r\n"
"ktKvaHcczHV2efHJagRHCbqR/hMGmX9+TKj5+fE80HyhgZgkhnIxNwmy5x6zLM0j\r\n"
"vf/xumvmTMhLi1p2fzlJhqVSv2p4P4wktxGiUYrE+pFNff49JlFrzFyu6QNRvs3o\r\n"
"SwIDAQAB\r\n"
"-----END PUBLIC KEY-----\r\n";
static const char g_client_private[]= \
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEA9HKy9M5zpGqZDSx0+dI+CQSPfOTc7gB5ZQHBBqVGYQ0sTE6T\r\n"
"6Rx22EoWZkJcQMKiUGLfuJBAsuSGAl7H8Isc9DYXjuhciviZi/AdntC35VbaOhIl\r\n"
"Rw9GvSenCzqoVMiAv9A0cJEgg9jpiA8KYBlN0vzSBci5VAWXyULHn1GYtHbbkGYl\r\n"
"Ht4DSceM9Itp32lfBXiKEEQ7fv6hzciV/cE1ktKvaHcczHV2efHJagRHCbqR/hMG\r\n"
"mX9+TKj5+fE80HyhgZgkhnIxNwmy5x6zLM0jvf/xumvmTMhLi1p2fzlJhqVSv2p4\r\n"
"P4wktxGiUYrE+pFNff49JlFrzFyu6QNRvs3oSwIDAQABAoIBABPV8tuWnR+sXrsI\r\n"
"7n0PKGWG7zXNRHgzpaYinE0zwCBD92JBxhLWcnZenK6zl9TRdmZbf5f0bKuXi6BJ\r\n"
"nKPSRgkzEnpQNKI6Rgtq8pxeh22evJqVulynyq+ytgVdPyeSFZB7DqZLwH2yblDS\r\n"
"F59wrjX+ufQqM4+z3HtCZfxxnJiYG+FUYxNCxnXFL5KQYMQRmySEYK4s6B8Xwzj3\r\n"
"DiiK3nopnuByl8MH/Xj0SWpHNKd2+CcbaucSRVWtkiv10MSuO+dSq5qXqu82Q7g+\r\n"
"tBo3q2lFMXfDWEp4uF5c9JfiSAYFhJ5DOO3Qn4+9NNYzAPHSYTeXoiHUvXz8/YuZ\r\n"
"h9czi6ECgYEA/5kLXJfTd24h4+pRGpqL6VOwkAz6mvhB+mxX/gopUXMLvnCAEui6\r\n"
"yRHYgdnbe54ZCJczL1f9K0bF8mtWhwyAX7bEoROajycL/yjPoIFsyhgR1aP6Eo+O\r\n"
"LXvl5kZYlDr0hYIfCMRFE8mK27NCtJLc/v5bz9KAx78S1uQ+CUq3nuMCgYEA9NUp\r\n"
"2u7uWcSOxy/HMeIRMsayQbZh3+BgTyfy/poaTA2sIFVhB4Aw4YsPDnGcFt6B4UyD\r\n"
"5WKSYgttYvJAXgiVHc5cX9yW3I5w5o2Si9ZsTm9P5DN7KdHIFdMfT/cUH9wIGA6b\r\n"
"rEdPR5h6IsimpiwLlGWZS0VtgizwwxkhKHRPJXkCgYEAiGyDp/RRppld9WyHxY9f\r\n"
"xjYpM3n5+OUxMhzKCooZ66NgTVF2yqD1sYSY7xX9FEoaSn+JjmeEdajc1rt7/7Eo\r\n"
"g+3lnaP9BxNsT9DUoXLzQhr+ye0v+OAunxBWcUZy2QtuoCQHtIIt7JXSS9mubPtG\r\n"
"gcP59T8CBZtyzsgcZJFA2MkCgYBmSTg3U2MY9dmAtiFOYdg1knWepoe1dJRH/bSz\r\n"
"UcZyx7OFvqXO8Gt7jxE3Q5empkc0zTa9erUVaRjGIddqccN6tGA7XHlao9ffOn5j\r\n"
"Fa4iBrcl66wlCJPDKnUGf39U4CQxiMjY7OHeU4GTTqjw+63KzbeBjvv1Srk7iSAl\r\n"
"OpJmYQKBgQCguIVqUbjHI0ZmPAyuAWDovPhRiUojnfdIcqor3wgHjY2qYLHd09GN\r\n"
"U307pYIMbBecEavUirGpHQ/7ozbEgvoZxA810Q7on3cY/NhMY3ZjdesSaGvECtgA\r\n"
"9iEzHnZXd++zVJD6vJ6JkejThyb0Xr3oPLrz0sHJwbeoe7akDXFBFA==\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

#define CN_HASH_CONTENT_LEN   32
#define CN_SIGN_CONTENT_LEN   256

static int ota_signtest(int argc, const char *argv[])
{
    int ret = 0;
    sign_para_t  sign_context;
    sign_verify_para_t  sign_verify_context;

    uint8_t hash_msg[CN_HASH_CONTENT_LEN]= {0,1,2,3,4,5,6,7,8,9,1,2,3,100,231,1,};
    uint8_t sign_content[CN_SIGN_CONTENT_LEN];

    sign_context.client_private_key = (const char *)g_client_private;
    sign_context.client_private_key_pwd = NULL;
    sign_context.hash = hash_msg;
    sign_context.hash_len = sizeof(hash_msg);
    sign_context.sign = sign_content;
    sign_context.sign_len = sizeof(sign_content);
    ret = ota_sign(&sign_context);
    if(ret != 0)
    {
        LINK_LOG_ERROR("SIGN FAILED");
        return ret;
    }

    sign_verify_context.client_public_key = (const char *)g_client_public;
    sign_verify_context.hash = hash_msg;
    sign_verify_context.hash_len = sizeof(hash_msg);
    sign_verify_context.sign = sign_content;
    sign_verify_context.sign_len = sizeof(sign_content);
    ret = ota_sign_verify(&sign_verify_context);
    if(ret != 0)
    {
        LINK_LOG_ERROR("SIGN VERIFY FAILED");
        return ret;
    }

    LINK_LOG_DEBUG("SIGN AND SIGN_VERIFY MATCH");
    return ret;
}

OSSHELL_EXPORT_CMD(ota_signtest,"otasign","otasign");



