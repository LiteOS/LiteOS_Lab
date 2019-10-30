/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include <kconfig.h>

#include CONFIG_STM32_HAL_H

#include <config.h>
#include <hal_uart.h>
#include <hal_int.h>
#include <init.h>
#include <symn2c.h>
#include <warn.h>

#include <lwip/netif.h>
#include <lwip/etharp.h>

#include <ethif.h>

/* macros */

/* typedefs */

struct stm32_eth
    {
    ETH_HandleTypeDef  heth;
    struct ethif       ethif;
    uint8_t            mac_addr [6];
    ETH_DMADescTypeDef rxtbl [ETH_RXBUFNB];
    uint8_t            rxbuf [ETH_RXBUFNB][ETH_RX_BUF_SIZE];
    ETH_DMADescTypeDef txtbl [ETH_TXBUFNB];
    uint8_t            txbuf [ETH_RXBUFNB][ETH_RX_BUF_SIZE];
    unsigned int       irqn;
    };

/* locals */

#ifdef CONFIG_STM32_ETH1
static struct stm32_eth stm32_eth1;
#endif

static int8_t __low_level_output (struct ethif * ethif, struct pbuf * p)
    {
    struct stm32_eth        * eth  = container_of (ethif, struct stm32_eth, ethif);
    ETH_HandleTypeDef       * heth = &eth->heth;
    err_t                     errval;
    struct pbuf             * q;
    uint8_t                 * buffer;
    __IO ETH_DMADescTypeDef * DmaTxDesc;
    uint32_t                  framelength     = 0;
    uint32_t                  bufferoffset    = 0;
    uint32_t                  byteslefttocopy = 0;
    uint32_t                  payloadoffset   = 0;

    buffer       = (uint8_t *) (heth->TxDesc->Buffer1Addr);
    DmaTxDesc    = heth->TxDesc;
    bufferoffset = 0;

    /* copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
        {
        /* Is this buffer available? If not, goto error */
        if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t) RESET)
            {
            errval = ERR_USE;
            goto error;
            }

        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len;
        payloadoffset   = 0;

        /* Check if the length of data to copy is bigger than Tx buffer size*/
        while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE)
            {
            /* Copy data to Tx buffer*/
            memcpy ((uint8_t *) ((uint8_t *) buffer + bufferoffset),
                    (uint8_t *) ((uint8_t *) q->payload + payloadoffset),
                    (ETH_TX_BUF_SIZE - bufferoffset));

            /* Point to next descriptor */
            DmaTxDesc = (ETH_DMADescTypeDef *) (DmaTxDesc->Buffer2NextDescAddr);

            /* Check if the buffer is available */
            if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t) RESET)
                {
                errval = ERR_USE;
                goto error;
                }

            buffer = (uint8_t *) (DmaTxDesc->Buffer1Addr);

            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset   = payloadoffset   + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength     = framelength     + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset    = 0;
        }

        /* Copy the remaining bytes */
        memcpy((uint8_t *) ((uint8_t *) buffer + bufferoffset),
               (uint8_t *) ((uint8_t *) q->payload + payloadoffset),
               byteslefttocopy);

        bufferoffset = bufferoffset + byteslefttocopy;
        framelength  = framelength  + byteslefttocopy;
        }

    ethif_tx_size_update (ethif, framelength);

    /* Prepare transmit descriptors to give to DMA */
    (void) HAL_ETH_TransmitFrame (heth, framelength);

    errval = ERR_OK;

error:

    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if ((heth->Instance->DMASR & ETH_DMASR_TUS) != (uint32_t) RESET)
        {
        /* Clear TUS ETHERNET DMA flag */
        heth->Instance->DMASR   = ETH_DMASR_TUS;

        /* Resume DMA transmission*/
        heth->Instance->DMATPDR = 0;
        }

    return errval;
    }

static struct pbuf * __low_level_input (struct ethif * ethif)
    {
    struct stm32_eth        * eth  = container_of (ethif, struct stm32_eth, ethif);
    ETH_HandleTypeDef       * heth = &eth->heth;
    struct pbuf             * p = NULL;
    struct pbuf             * q = NULL;
    uint16_t                  len = 0;
    uint8_t                 * buffer;
    __IO ETH_DMADescTypeDef * dmarxdesc;
    uint32_t                  bufferoffset    = 0;
    uint32_t                  payloadoffset   = 0;
    uint32_t                  byteslefttocopy = 0;
    uint32_t                  i = 0;

    /* get received frame */
    if (HAL_ETH_GetReceivedFrame (heth) != HAL_OK)
        {
	    return NULL;
        }

    /* Obtain the size of the packet and put it into the "len" variable. */
    len    = (uint16_t)  heth->RxFrameInfos.length;
    buffer = (uint8_t *) heth->RxFrameInfos.buffer;

    ethif_rx_size_update (ethif, len);

    if (len > 0)
        {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc (PBUF_RAW, len, PBUF_POOL);
        }

    if (p != NULL)
        {
        dmarxdesc    = heth->RxFrameInfos.FSRxDesc;
        bufferoffset = 0;

        for (q = p; q != NULL; q = q->next)
            {
            byteslefttocopy = q->len;
            payloadoffset   = 0;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
            while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE)
                {
                /* Copy data to pbuf */
                memcpy ((uint8_t *) ((uint8_t *)q->payload + payloadoffset),
                        (uint8_t *) ((uint8_t *)buffer + bufferoffset),
                        (ETH_RX_BUF_SIZE - bufferoffset));

                /* Point to next descriptor */
                dmarxdesc = (ETH_DMADescTypeDef*) (dmarxdesc->Buffer2NextDescAddr);
                buffer    = (uint8_t *) (dmarxdesc->Buffer1Addr);

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset   = payloadoffset   + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset    = 0;
                }

            /* Copy remaining data in pbuf */
            memcpy ((uint8_t *) ((uint8_t *) q->payload + payloadoffset),
                    (uint8_t *) ((uint8_t *) buffer + bufferoffset),
                    byteslefttocopy);

            bufferoffset = bufferoffset + byteslefttocopy;
            }
        }

    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dmarxdesc = heth->RxFrameInfos.FSRxDesc;

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (i = 0; i < heth->RxFrameInfos.SegCount; i++)
        {
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;
        dmarxdesc          = (ETH_DMADescTypeDef*)(dmarxdesc->Buffer2NextDescAddr);
        }

    /* Clear Segment_Count */
    heth->RxFrameInfos.SegCount = 0;

    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if ((heth->Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t) RESET)
        {
        /* Clear RBUS ETHERNET DMA flag */
        heth->Instance->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        heth->Instance->DMARPDR = 0;
        }

    return p;
    }

void HAL_ETH_MspInit (ETH_HandleTypeDef * ethHandle)
    {
    GPIO_InitTypeDef io;

#if 1
    __HAL_RCC_ETH_CLK_ENABLE ();
    __HAL_RCC_GPIOC_CLK_ENABLE ();
    __HAL_RCC_GPIOH_CLK_ENABLE ();
    __HAL_RCC_GPIOA_CLK_ENABLE ();
    __HAL_RCC_GPIOB_CLK_ENABLE ();
    __HAL_RCC_GPIOG_CLK_ENABLE ();
    __HAL_RCC_GPIOI_CLK_ENABLE ();
#else
    __HAL_RCC_ETH_CLK_ENABLE ();
    __HAL_RCC_ETHMAC_CLK_ENABLE ();
    __HAL_RCC_ETHMACTX_CLK_ENABLE ();
    __HAL_RCC_ETHMACRX_CLK_ENABLE ();
#endif

    if (ethHandle->Instance == ETH)
        {
        /* USER CODE BEGIN ETH_MspInit 0 */

        /* USER CODE END ETH_MspInit 0 */
        /* Enable Peripheral clock */

        #if 1

        /**ETH GPIO Configuration
             PC1     ------> ETH_MDC
             PA1     ------> ETH_REF_CLK
             PA2     ------> ETH_MDIO
             PA7     ------> ETH_CRS_DV
             PC4     ------> ETH_RXD0
             PC5     ------> ETH_RXD1
             PB11     ------> ETH_TX_EN
             PG13     ------> ETH_TXD0
             PG14     ------> ETH_TXD1
             PI1      ------> ETH_NRST
             */

        io.Pin   = GPIO_PIN_1;
        io.Mode  = GPIO_MODE_OUTPUT_PP;
        io.Pull  = GPIO_NOPULL;
        io.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init (GPIOI, &io);


        HAL_GPIO_WritePin (GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
        task_delay (CONFIG_SYS_TICK_HZ);
        HAL_GPIO_WritePin (GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
        task_delay (CONFIG_SYS_TICK_HZ);

        io.Pin       = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
        io.Mode      = GPIO_MODE_AF_PP;
        io.Pull      = GPIO_NOPULL;
        io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        io.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init (GPIOC, &io);

        io.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
        io.Mode      = GPIO_MODE_AF_PP;
        io.Pull      = GPIO_NOPULL;
        io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        io.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init (GPIOA, &io);

        io.Pin       = GPIO_PIN_11;
        io.Mode      = GPIO_MODE_AF_PP;
        io.Pull      = GPIO_NOPULL;
        io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        io.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init (GPIOB, &io);

        io.Pin       = GPIO_PIN_13 | GPIO_PIN_14;
        io.Mode      = GPIO_MODE_AF_PP;
        io.Pull      = GPIO_NOPULL;
        io.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        io.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init (GPIOG, &io);
        #else

        io.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
        io.Mode      = GPIO_MODE_AF_PP;
        io.Pull      = GPIO_NOPULL;
        io.Speed     = GPIO_SPEED_FREQ_HIGH;
        io.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init (GPIOA, &io);

        io.Pin       = GPIO_PIN_11;
        HAL_GPIO_Init (GPIOB, &io);

        io.Pin       = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
        HAL_GPIO_Init (GPIOC, &io);

        io.Pin       = GPIO_PIN_14 | GPIO_PIN_13;
        HAL_GPIO_Init (GPIOG, &io);
        #endif
        }
    }

static void __stm32_eth_handler (uint32_t arg)
    {
    struct stm32_eth * eth = (struct stm32_eth *) arg;

    HAL_ETH_IRQHandler (&eth->heth);
    }

void HAL_ETH_RxCpltCallback (ETH_HandleTypeDef * heth)
    {
    struct stm32_eth * eth = container_of (heth, struct stm32_eth, heth);

	ethif_rx (&eth->ethif);
    }

static int __stm32_eth_init (struct stm32_eth * eth, const char * name,
                             struct netif_ops * ops, const char * mac,
                             unsigned int irqn)
    {
    eth->irqn = irqn;

    if (unlikely (ethif_register (&eth->ethif, name, ops, mac) != 0))
        {
        return -1;
        }

    return 0;
    }

static int __low_level_start (struct ethif * ethif)
    {
    struct stm32_eth * eth = container_of (ethif, struct stm32_eth, ethif);

    /* Init ETH */
    eth->heth.Instance             = ETH;   // TODO, by argument
    eth->heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
    eth->heth.Init.PhyAddress      = 0;

    eth->heth.Init.MACAddr         = ethif_getmac (ethif);
    eth->heth.Init.RxMode          = ETH_RXINTERRUPT_MODE;
    eth->heth.Init.ChecksumMode    = ETH_CHECKSUM_BY_HARDWARE;
    eth->heth.Init.MediaInterface  = ETH_MEDIA_INTERFACE_RMII;

    if (HAL_ETH_Init (&eth->heth) != HAL_OK)
        {
        WARN ("HAL_ETH_Init fail!");
        return -1;
        }

    /* Initialize Tx Descriptors list: Chain Mode */

    (void) HAL_ETH_DMATxDescListInit (&eth->heth, eth->txtbl, &eth->txbuf [0][0],
                                      ETH_TXBUFNB);

    /* Initialize Rx Descriptors list: Chain Mode  */

    (void) HAL_ETH_DMARxDescListInit (&eth->heth, eth->rxtbl, &eth->rxbuf [0][0],
                                      ETH_RXBUFNB);

    (void) HAL_ETH_Start (&eth->heth);

    if (hal_int_connect (eth->irqn, (hal_int_handler_t) __stm32_eth_handler,
                         (uintptr_t) eth) != 0)
        {
        return -1;
        }

    if (hal_int_enable  (eth->irqn) != 0)
        {
        return -1;
        }

    if (hal_int_setprio (eth->irqn, 0))
        {
        return -1;
        }

    ethif->netif.flags |= NETIF_FLAG_LINK_UP;

    return 0;
    }

static struct netif_ops __stm32_eth_ops =
    {
    __low_level_input,
    __low_level_output,
    __low_level_start
    };

static int stm32_eth_init (void)
    {
    WARN_ON (__stm32_eth_init (&stm32_eth1, "eth1", &__stm32_eth_ops,
                               CONFIG_STM32_ETH1_MAC, ETH_IRQn) != 0,
             return -1,
             "Fail to init %s!", "eth1");

    return 0;
    }

/* there are task_delay in the initialization routine, so do it in user phase for now */

MODULE_INIT (user, stm32_eth_init);

