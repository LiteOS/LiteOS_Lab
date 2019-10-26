/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *      http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#ifndef __ETHIF_H__
#define __ETHIF_H__

#include <stdint.h>

#include <lwip/err.h>
#include <lwip/netif.h>

#include <defer.h>
#include <mutex.h>
#include <list.h>

#define NETIF_MTU                       (1500)

#define NETIF_IN_TASK_STACK_SIZE        (1024)
#define NETIF_IN_TASK_PRIORITY          (4)

#define NETIF_OUT_TASK_STACK_SIZE       (1024)
#define NETIF_OUT_TASK_PRIORITY         (4)

/* typedefs */

struct ethif;

struct netif_ops
    {
    struct pbuf * (* input)  (struct ethif *);
    int8_t        (* output) (struct ethif *, struct pbuf *);
    int           (* start)  (struct ethif *);
    };

struct ethif
    {
    dlist_t          node;
    struct netif     netif;
    const char     * name;
    const char     * mac;

    mutex_t          lock;

    deferred_job_t   job;

#ifdef CONFIG_CMDER
    uint64_t         tx_pkts;
    uint64_t         tx_size;
    uint64_t         rx_pkts;
    uint64_t         rx_size;
    uint64_t         rx_drop;
#endif

    struct netif_ops * ops;
    };

/* inlines */

#ifdef CONFIG_CMDER
static inline void ethif_tx_pkts_update (struct ethif * ethif)
    {
    ethif->tx_pkts++;
    }

static inline void ethif_tx_size_update (struct ethif * ethif, uint32_t len)
    {
    ethif->tx_size += len;
    }

static inline void ethif_rx_pkts_update (struct ethif * ethif)
    {
    ethif->rx_pkts++;
    }

static inline void ethif_rx_size_update (struct ethif * ethif, uint32_t len)
    {
    ethif->rx_size += len;
    }

static inline void ethif_rx_drop_update (struct ethif * ethif)
    {
    ethif->rx_drop++;
    }
#else
#define ethif_tx_pkts_update(...)
#define ethif_tx_size_update(...)
#define ethif_rx_pkts_update(...)
#define ethif_rx_size_update(...)
#define ethif_rx_drop_update(...)
#endif

/* externs */

extern void   ethif_rx       (struct ethif *);
extern int    ethif_register (struct ethif *, const char *, struct netif_ops *,
                              const char *);
extern int    ifconfig       (const char *, const char *, const char *,
                              const char *, const char *);
extern u8_t * ethif_getmac   (struct ethif *);

#endif /* __ETHIF_H__ */

