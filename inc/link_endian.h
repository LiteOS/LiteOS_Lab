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

#ifndef LITEOS_LAB_IOT_LINK_INC_LINK_ENDIAN_H_
#define LITEOS_LAB_IOT_LINK_INC_LINK_ENDIAN_H_


//define the normal addres function
#define swaps(value) ((((value)&((uint16_t)0xff00))>>8)|(((value)&((uint16_t)0x00ff))<<8))

#define swapl(value)  ((((value)&((uint32_t)0xff000000))>>24)|(((value)&((uint32_t)0xff0000))>>8)|\
                      (((value)&((uint32_t)0xff00))<<8)|(((value)&((uint32_t)0xff))<<24))

#if cfg_endian_big

#define htoles(value)  swaps(value)
#define htolel(value)  swapl(value)
#define htobes(value)  (value)
#define htobel(value)  (value)

#else
#define htoles(value)  (value)
#define htolel(value)  (value)
#define htobes(value)  swaps(value)
#define htobel(value)  swapl(value)
#endif

#ifndef htons
#define htons      htobes      //translate the host endian to network endian (2 Bytes)
#endif

#ifndef htonl
#define htonl      htobel      //translate the host endian to network endian (4 Bytes)
#endif

#ifndef ntohs
#define ntohs      htobes      //translate the network endian to host endian (2 Bytes)
#endif

#ifndef ntohl
#define ntohl      htobel      //translate the network endian to host endian (4 Bytes)
#endif



#endif /* LITEOS_LAB_IOT_LINK_INC_LINK_ENDIAN_H_ */
