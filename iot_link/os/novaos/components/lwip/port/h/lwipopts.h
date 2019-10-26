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

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include <kconfig.h>

#define TCPIP_THREAD_NAME               CONFIG_TCPIP_TASK_NAME
#define TCPIP_THREAD_STACKSIZE          CONFIG_TCPIP_TASK_STACK_SIZE
#define TCPIP_THREAD_PRIO               CONFIG_TCPIP_TASK_PRIO

#define TCPIP_MBOX_SIZE                 CONFIG_TCPIP_MBOX_SIZE

#ifdef CONFIG_PPP_SUPPORT
#define PPP_SUPPORT                     1
#else
#define PPP_SUPPORT                     0
#endif

#ifdef CONFIG_IPV6
#define LWIP_IPV6                       1
#endif

#define NO_SYS                          0
#define LWIP_SOCKET                     (NO_SYS==0)
#define LWIP_NETCONN                    0
#define LWIP_NETIF_API                  0

#define SYS_LIGHTWEIGHT_PROT            1

/* UDP options */

#define LWIP_UDP                        1
#define LWIP_UDPLITE                    LWIP_UDP
#define UDP_TTL                         255

#define LWIP_IGMP                       LWIP_IPV4
#define LWIP_ICMP                       LWIP_IPV4

#define LWIP_SNMP                       LWIP_UDP
#define MIB2_STATS                      LWIP_SNMP
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_SNMP_V3                    (LWIP_SNMP)
#endif

#define LWIP_DNS                        LWIP_UDP
#define LWIP_MDNS_RESPONDER             LWIP_UDP

#define LWIP_NUM_NETIF_CLIENT_DATA      (LWIP_MDNS_RESPONDER)

#define LWIP_HAVE_LOOPIF                1
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_LOOPBACK_MAX_PBUFS         10

#define TCP_LISTEN_BACKLOG              1

#define LWIP_COMPAT_SOCKETS             1
#define LWIP_SO_RCVTIMEO                1
#define LWIP_SO_RCVBUF                  1

#define LWIP_TCPIP_CORE_LOCKING         1

#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#ifdef LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL              0
#define PPP_DEBUG                       LWIP_DBG_OFF
#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#define API_MSG_DEBUG                   LWIP_DBG_OFF
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#define DNS_DEBUG                       LWIP_DBG_OFF
#define AUTOIP_DEBUG                    LWIP_DBG_OFF
#define DHCP_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define IGMP_DEBUG                      LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#endif

#define LWIP_DBG_TYPES_ON               \
    (LWIP_DBG_ON | LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH | LWIP_DBG_HALT)


/* memory options */

#define MEM_ALIGNMENT                   4U
#define MEM_SIZE                        10240
#define MEMP_NUM_PBUF                   16
#define MEMP_NUM_RAW_PCB                3
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                5
#define MEMP_NUM_TCP_PCB_LISTEN         8
#define MEMP_NUM_TCP_SEG                16
#define MEMP_NUM_SYS_TIMEOUT            17
#define MEMP_NUM_NETBUF                 2
#define MEMP_NUM_NETCONN                10
#define MEMP_NUM_TCPIP_MSG_API          16
#define MEMP_NUM_TCPIP_MSG_INPKT        16

/* pbuf options */

#define PBUF_POOL_SIZE                  120
#define PBUF_POOL_BUFSIZE               256

/* TCP options */

#define LWIP_TCP                        1
#define TCP_TTL                         255

#define LWIP_ALTCP                      (LWIP_TCP)
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_ALTCP_TLS                  (LWIP_TCP)
#define LWIP_ALTCP_TLS_MBEDTLS          (LWIP_TCP)
#endif

#define TCP_QUEUE_OOSEQ                 1
#define TCP_MSS                         1024
#define TCP_SND_BUF                     2048
#define TCP_SND_QUEUELEN                (4 * TCP_SND_BUF/TCP_MSS)
#define TCP_SNDLOWAT                    (TCP_SND_BUF/2)
#define TCP_WND                         (20 * 1024)
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   4

/* ARP options */
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10
#define ARP_QUEUEING                    1

/* IP options */

#define IP_FORWARD                      1
#define IP_REASSEMBLY                   1
#define IP_REASS_MAX_PBUFS              (10 * ((1500 + PBUF_POOL_BUFSIZE - 1) / PBUF_POOL_BUFSIZE))
#define MEMP_NUM_REASSDATA              IP_REASS_MAX_PBUFS
#define IP_FRAG                         1
#define IPV6_FRAG_COPYHEADER            1

/* ICMP options */

#define ICMP_TTL                        255

/* DHCP options */

#define LWIP_DHCP                       LWIP_UDP
#define DHCP_DOES_ARP_CHECK             (LWIP_DHCP)

/* AUTOIP options */

#define LWIP_AUTOIP                     (LWIP_DHCP)
#define LWIP_DHCP_AUTOIP_COOP           (LWIP_DHCP && LWIP_AUTOIP)

/* RAW options */
#define LWIP_RAW                        1

/* statistics options */

#define LWIP_STATS                      1
#define LWIP_STATS_DISPLAY              1

#if LWIP_STATS
#define LINK_STATS                      1
#define IP_STATS                        1
#define ICMP_STATS                      1
#define IGMP_STATS                      1
#define IPFRAG_STATS                    1
#define UDP_STATS                       1
#define TCP_STATS                       1
#define MEM_STATS                       1
#define MEMP_STATS                      1
#define PBUF_STATS                      1
#define SYS_STATS                       1
#endif /* LWIP_STATS */

/* NETBIOS options */

#define LWIP_NETBIOS_RESPOND_NAME_QUERY 1

/* PPP options */

#if PPP_SUPPORT

#define NUM_PPP                         CONFIG_NUM_PPP

#ifdef CONFIG_PPPOE_SUPPORT
#define PPPOE_SUPPORT                   1
#endif

#ifdef CONFIG_PPPOS_SUPPORT
#define PPPOS_SUPPORT                   1
#endif

#ifdef CONFIG_PAP_SUPPORT
#define PAP_SUPPORT                     1
#endif

#ifdef CONFIG_CHAP_SUPPORT
#define CHAP_SUPPORT                    1
#endif

#ifdef CONFIG_MSCHAP_SUPPORT
#define MSCHAP_SUPPORT                  1
#endif

#ifdef CONFIG_CBCP_SUPPORT
#define CBCP_SUPPORT                    1
#endif

#ifdef CONFIG_CCP_SUPPORT
#define CCP_SUPPORT                     1
#endif

#ifdef CONFIG_VJ_SUPPORT
#define VJ_SUPPORT                      1
#endif

#ifdef CONFIG_MD5_SUPPORT
#define MD5_SUPPORT                     1
#endif

#endif /* PPP_SUPPORT */

#ifdef CONFIG_HARDWARE_CHECKSUM
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0
#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_GEN_ICMP               0
#endif

#endif /* __LWIPOPTS_H__ */
