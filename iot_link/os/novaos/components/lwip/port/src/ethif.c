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

#include <string.h>

#include <config.h>
#include <msg_queue.h>
#include <bug.h>
#include <warn.h>
#include <init.h>
#include <mutex.h>

#include <lwip/opt.h>
#include <lwip/netif.h>
#include <lwip/etharp.h>
#include <lwip/timeouts.h>
#include <netif/ethernet.h>

#include <ethif.h>

#ifdef CONFIG_CMDER
#include <cmder.h>
#endif

/* locals */

static dlist_t __ethifs = DLIST_INIT (__ethifs);
static mutex_t __ethifs_lock;

static struct ethif * __get_ethif (const char * name)
    {
    dlist_t      * itr;
    struct ethif * ethif;

    mutex_lock (&__ethifs_lock);

    dlist_foreach (itr, &__ethifs)
        {
        ethif = container_of (itr, struct ethif, node);

        // it is safe to use strcmp here

        if (strcmp (name, ethif->name) == 0)
            {
            mutex_unlock (&__ethifs_lock);
            return ethif;
            }
        }

    mutex_unlock (&__ethifs_lock);

    return NULL;
    }

static void __ethif_input (uintptr_t arg)
    {
    struct ethif     * ethif = (struct ethif *) arg;
    struct netif     * netif = &ethif->netif;
    struct netif_ops * ops   = (struct netif_ops *) ethif->ops;
    struct pbuf      * buf;

    while ((buf = ops->input (ethif)) != NULL)
        {
        if (netif->input (buf, netif) != ERR_OK)
            {
            LWIP_DEBUGF (NETIF_DEBUG, ("__ethif_input: IP input error\n"));
            pbuf_free (buf);

            ethif_rx_drop_update (ethif);

            break;
            }

        ethif_rx_pkts_update (ethif);
        }
    }

/**
 * ethif_rx - notify rx event
 * @ethif: the ethif object
 *
 * return: NA
 */

void ethif_rx (struct ethif * ethif)
    {
    deferred_job_sched (&ethif->job);
    }

static void __arp_timer (void * arg)
    {
    etharp_tmr ();
    sys_timeout (ARP_TMR_INTERVAL, __arp_timer, NULL);
    }

#if LWIP_ARP || LWIP_ETHERNET

static int __get_mac_num (char ch)
    {
    if ((ch >= '0') && (ch <= '9'))
        {
        return ch - '0';
        }

    /* upper -> lower, lower keep no changed */

    ch |= ('a' - 'A');

    if ((ch >= 'a') && (ch <= 'f'))
        {
        return ch - 'a' + 10;
        }

    return -1;
    }

static int __get_mac (u8_t * mac, const char * str)
    {
    int  i;
    int  m;
    int  n;
    char s = str [2];   // split char, "-" or ":"

    if ((s != ':') && (s != '-'))
        {
        return -1;
        }

    for (i = 0; i < 6; i++)
        {
        if (str [2] != s && str [2] != '\0')
            {
            return -1;
            }

        if (((m = __get_mac_num (*str++)) == -1) ||
            ((n = __get_mac_num (*str++)) == -1))
            {
            return -1;
            }

        *mac++ = (u8_t) ((m << 4) | n);

        str++;
        }

    return i == 6 && str [-1] == '\0' ? 0 : -1;
    }

#endif

static int __get_ip_n (const char * str, const char * e)
    {
    int temp = 0;

    do
        {
        if ((*str < '0') || (*str > '9'))
            {
            return -1;
            }

        temp = temp * 10 + *str - '0';
        } while (++str != e);

    return temp;
    }

static u32_t __get_ip (const char * str)
    {
    const char * e;
    u32_t        addr = 0;
    int          temp;
    int          i;

    for (i = 0; i < 4; i++, str = e + 1)
        {
        if ((e = strchr (str, i != 3 ? '.' : '\0')) == NULL)
            {
            return 0;
            }

        if ((e == str) || (e - str) > 3)
            {
            return 0;
            }

        if ((temp = __get_ip_n (str, e)) == -1)
            {
            return 0;
            }

        if (temp >= 256)
            {
            return 0;
            }

        addr |= ((u32_t) temp) << ((3 - i) << 3);
        }

    return PP_HTONL (addr);
    }

static int8_t __ethif_output (struct netif * netif, struct pbuf * p)
    {
    struct ethif * ethif = container_of (netif, struct ethif, netif);

    ethif_tx_pkts_update (ethif);

    return ethif->ops->output (ethif, p);
    }

static err_t __ethif_init (struct netif * netif)
    {
    struct ethif * eth;

    eth = container_of (netif, struct ethif, netif);

#if LWIP_NETIF_HOSTNAME
    netif->hostname = "lwip";
#endif

    netif->name [0] = 'i';
    netif->name [1] = 'f';

    netif->output     = etharp_output;
    netif->linkoutput = __ethif_output;

#if LWIP_ARP || LWIP_ETHERNET
    netif->hwaddr_len = ETH_HWADDR_LEN;

    netif->mtu = CONFIG_MTU_VALUE;

    WARN_ON (__get_mac (netif->hwaddr, eth->mac) != 0,
             return -1,
             "MAC address (%s) format not correct!", eth->mac);

    /* Accept broadcast address and ARP traffic */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */

#if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#else
    netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */

    WARN_ON (eth->ops->start (eth) != 0,
             return ERR_IF,
             "ops->start fail!");

    sys_timeout (ARP_TMR_INTERVAL, __arp_timer, NULL);

    return ERR_OK;
    }

/**
 * ifconfig - configure an ethif
 * @name:     the name of the ethif
 * @op:       "up" or "down"
 * @ip_str:   the ip address string
 * @mask_str: the mask string
 * @gw_str:   the gateway address string
 *
 * return: 0 on success, negtive value on error
 */

int ifconfig (const char * name, const char * op, const char * ip_str,
              const char * mask_str, const char * gw_str)
    {
    static bool    has_default = false;
    bool           is_up;
    struct ethif * ethif = __get_ethif (name);
    ip4_addr_t     ip4_addr;

    if (ethif == NULL)
        {
        // TODO: errno
        return -1;
        }

    is_up = netif_is_up (&ethif->netif);

    if (is_up)
        {
        netif_set_down (&ethif->netif);
        }

    if (ip_str != NULL)
        {
        ip4_addr.addr = __get_ip (ip_str);
        netif_set_ipaddr (&ethif->netif, &ip4_addr);
        }

    if (mask_str != NULL)
        {
        ip4_addr.addr = __get_ip (mask_str);
        netif_set_netmask (&ethif->netif, &ip4_addr);
        }

    if (gw_str != NULL)
        {
        ip4_addr.addr = __get_ip (gw_str);
        netif_set_gw (&ethif->netif, &ip4_addr);
        }

    if ((op != NULL) && (strcmp (op, "down") == 0))
        {
        return 0;
        }

    if (is_up || ((op != NULL) && (strcmp (op, "up") == 0)))
        {
        netif_set_up (&ethif->netif);

        if (!has_default)
            {
            netif_set_default (&ethif->netif);
            has_default = true;
            }
        }

    return 0;
    }

/**
 * ethif_register - register an ethif
 * @ethif:  the ethif object
 * @name:   the name of the ethif
 * @ops:    the operations
 * @mac:    the mac address string, format aa-bb-cc-xx-yy-zz or aa:bb:cc:xx:yy:zz
 *
 * return: 0 on success, negtive value on error
 */

int ethif_register (struct ethif * ethif, const char * name, struct netif_ops * ops,
                    const char * mac)
    {
    BUG_ON (ethif       == NULL, "Invalid ethif!");
    BUG_ON (name        == NULL, "Invalid ethif name!");
    BUG_ON (ops         == NULL, "Invalid ethif ops!");
    BUG_ON (mac         == NULL, "Invalid ethif MAC address!");
    BUG_ON (ops->input  == NULL, "Invalid ethif ops!");
    BUG_ON (ops->output == NULL, "Invalid ethif ops!");
    BUG_ON (ops->start  == NULL, "Invalid ethif ops!");

    memset (ethif, 0, sizeof (struct ethif));

    ethif->mac  = mac;
    ethif->name = name;
    ethif->ops  = ops;

    if (netif_add (&ethif->netif, NULL, NULL, NULL, NULL, __ethif_init,
                   ethernet_input) == NULL)
        {
        WARN ("Fail to add ethif!");

        return -1;
        }

    if (unlikely (deferred_job_init (&ethif->job, __ethif_input, (uintptr_t) ethif) != 0))
        {
        return -1;
        }

    if (unlikely (mutex_init (&ethif->lock) != 0))
        {
        return -1;
        }

    if (unlikely (mutex_lock (&__ethifs_lock) != 0))
        {
        mutex_destroy (&ethif->lock);
        return -1;
        }

    dlist_add_tail (&__ethifs, &ethif->node);

    (void) mutex_unlock (&__ethifs_lock);

    return 0;
    }

/**
 * ethif_getmac - get the mac address from an ethif
 * @ethif:  the ethif object
 *
 * return: the address of the mac address held in the ethif
 */

u8_t * ethif_getmac (struct ethif * ethif)
    {
    WARN_ON (ethif == NULL,
             return NULL,
             "Invalid ethif!");

    return ethif->netif.hwaddr;
    }

#ifdef CONFIG_CMDER

static void __show_size (cmder_t * cmder, const char * type, uint64_t size)
    {
    uint32_t     g = 1024 * 1024 * 1024;
    uint32_t     m = 1024 * 1024;
    uint32_t     k = 1024;
    uint32_t     x;
    const char * xib;

    if (size >= g)
        {
        x   = g;
        xib = "GiB";
        }
    else if (size >= m)
        {
        x   = m;
        xib = "MiB";
        }
    else
        {
        x   = k;
        xib = "KiB";
        }

    cmder_printf (cmder, "%8s%s size: %lld (%lld.%lld %s)\n", "", type, size,
                  size / x, ((size & (x - 1)) * 10) / x, xib);
    }

static void __show_ethif (cmder_t * cmder, struct ethif * ethif)
    {
    uint32_t ip   = PP_NTOHL (ethif->netif.ip_addr.addr);
    uint32_t mask = PP_NTOHL (ethif->netif.netmask.addr);
    uint32_t gw   = PP_NTOHL (ethif->netif.gw.addr);

    cmder_printf (cmder, "%-8s%s hwaddr %02x:%02x:%02x:%02x:%02x:%02x\n", ethif->name,
                  netif_is_up (&ethif->netif) ? "up" : "down",
                  ethif->netif.hwaddr [0], ethif->netif.hwaddr [1],
                  ethif->netif.hwaddr [2], ethif->netif.hwaddr [3],
                  ethif->netif.hwaddr [4], ethif->netif.hwaddr [5]);
    cmder_printf (cmder, "%8sinet addr:%d.%d.%d.%d mask:%d.%d.%d.%d\n", "",
                  ip   >> 24, (ip   >> 16) & 0xff, (ip   >> 8) & 0xff, ip   & 0xff,
                  mask >> 24, (mask >> 16) & 0xff, (mask >> 8) & 0xff, mask & 0xff);
    cmder_printf (cmder, "%8sgate:%d.%d.%d.%d\n", "",
                  gw   >> 24, (gw   >> 16) & 0xff, (gw   >> 8) & 0xff, gw   & 0xff);

    cmder_printf (cmder, "%8srx packets: %lld dropped: %lld\n", "", ethif->rx_pkts, ethif->rx_drop);
    cmder_printf (cmder, "%8stx packets: %lld\n", "", ethif->tx_pkts);
    __show_size (cmder, "rx", ethif->rx_size);
    __show_size (cmder, "tx", ethif->tx_size);
    }

static void __show_all_ethif (cmder_t * cmder)
    {
    dlist_t      * itr;
    struct ethif * ethif;

    if (mutex_lock (&__ethifs_lock) != 0)
        {
        cmder_printf (cmder, "fail to iterator the ethif list!\n");
        return;
        }

    dlist_foreach (itr, &__ethifs)
        {
        ethif = container_of (itr, struct ethif, node);

        __show_ethif (cmder, ethif);
        }

    mutex_unlock (&__ethifs_lock);

    return;
    }

static bool __is_ip_addr (const char * str)
    {
    if (str == NULL)
        {
        return false;
        }

    return __get_ip (str) == 0 ? false : true;
    }

static int __cmd_ifconfig (cmder_t * cmder, int argc, char * argv [])
    {
    const char   * name;
    const char   * op     = NULL;
    const char   * ip [3] = { NULL, NULL, NULL };
    int            i;
    struct ethif * ethif;

    if (argc == 1)
        {
        __show_all_ethif (cmder);
        return 0;
        }

    argv++;
    argc--;

    name = *argv;

    ethif = __get_ethif (name);

    if (ethif == NULL)
        {
        cmder_printf (cmder, "ethif not exist!\n");
        return -1;
        }

    argv++;
    argc--;

    /* ifconfig eth1 */

    if (argc == 0)
        {
        __show_ethif (cmder, ethif);
        return 0;
        }

    if ((strcmp (*argv, "up") == 0) || (strcmp (*argv, "down") == 0))
        {
        op = *argv++;
        argc--;
        }

    for (i = 0; i < 3 && argc != 0; i++, argc--, argv++)
        {
        if (__is_ip_addr (*argv))
            {
            ip [i] = *argv;
            }
        }

    if (op == NULL && ip [0] == NULL&& ip [1] == NULL&& ip [2] == NULL)
        {
        cmder_printf (cmder, "no valid arguments, do nothing!\n");
        return -1;
        }

    return ifconfig (name, op, ip [0], ip [1], ip [2]);
    }

CMDER_CMD_DEF ("ifconfig", "view and change the configuration of the network interfaces",
               __cmd_ifconfig);

#endif

static int ethif_lib_init (void)
    {
    if (unlikely (mutex_init (&__ethifs_lock) != 0))
        {
        return -1;
        }

    return 0;
    }

MODULE_INIT (user, ethif_lib_init);
