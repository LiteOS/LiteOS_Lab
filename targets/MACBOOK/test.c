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
 *  2019-06-02 00:31  zhangqianfu  The first version
 *
 */


#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>



static int getvalue(sem_t *sem)
{

    int ret =0;

    while(0 == sem_trywait(sem))
    {
        ret ++;
    }

    return ret;
}


int sem_main(int argc, const char *argv[])
{
    sem_t *s1;
    sem_t *s2;


   s1 = sem_open("he1",O_CREAT, S_IRUSR | S_IWUSR, 10);
   s2 = sem_open("he2",O_CREAT, S_IRUSR | S_IWUSR, 7);


   printf("address:0x%08x  value:0x%08x\n\r",(unsigned int)s1,getvalue(s1));

   printf("address:0x%08x  value:0x%08x\n\r",(unsigned int)s2,getvalue(s2));


   sem_close(s1);
   sem_close(s2);

   return 0;

}

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc,const char *argv[])
{
    int ret;
    int fd;
    struct sockaddr_in address;
    int addrlen;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("49.4.85.232");
    address.sin_port = htons(5684);

    addrlen = sizeof(address);

    fd = socket(AF_INET,SOCK_DGRAM,0);

    if(fd != -1)
    {

        ret = connect(fd,&address,sizeof(address));
        printf("fd:%d addresslen:%d family:%d address:%s port:%d ret:%d\n\r",fd,\
                addrlen,address.sin_family,inet_ntoa(address.sin_addr),ntohs(address.sin_port),ret);
        close(fd);


    }


    return 0;
}

