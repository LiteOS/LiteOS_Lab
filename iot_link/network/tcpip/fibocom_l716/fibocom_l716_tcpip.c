#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __KEIL__
#include <unistd.h>
#endif

#include <sal.h>
#include <sal_imp.h>
#include <sal_define.h>
#include <sal_types.h>
#include <at.h>
#include <link_misc.h>
#include <link_endian.h>
#include <osal.h>
#include <iot_link_config.h>
#include <timeval.h>

#define CMDTIMEOUT      (6*1000)
#define RCVINDEX        "\r\n+MIPR"
#define CACHELEN        (1024 * 5)
#define MAX_NR_FDS      (7)

typedef struct {
    int               sockfd;
    int               domain;
    int               type;
    int               protocol;

    bool_t            connected;
    unsigned int      timeout;
    tag_ring_buffer_t l716_rcvring;

    char              oob_resp[CACHELEN];
    unsigned char     rcvbuf[CACHELEN];
} l716_sock_cb_t;

static l716_sock_cb_t * s_l716_cbs [MAX_NR_FDS] = { NULL, };

static osal_mutex_t         g_atcombo_lock;
static osal_mutex_t         g_iobufer_lock;


static bool_t __l716_atcmd(const char *cmd,const char *index,char *buf, int len)
{
    return at_command((const void *) cmd, strlen(cmd), index, buf, len, CMDTIMEOUT) >= 0;
}

static bool_t __l716_atcmd_simple(const char *cmd,const char *index)
{
    return __l716_atcmd(cmd, index, NULL, 0);
}

static void __l716_atcmd_noindex(const char *cmd)
{
    at_command((const void *) cmd, strlen(cmd), NULL, NULL, 0, CMDTIMEOUT);
}

static void __l716_atcmd_int_noindex(int i)
{
    char buff[12];

    snprintf(buff, 12, "%d", i);

    __l716_atcmd_noindex(buff);
}

static int __l716_poll_cmd(const char *cmd, const char *index, uint32_t timeout, int retry)
{
    do {
        if (at_command((const void *) cmd, strlen(cmd), index, NULL, 0, timeout) >= 0) {
            return 0;
        }
    } while (--retry != 0);

    return -1;
}

static unsigned int __atoi(unsigned char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }

    ch |= 0x20;

    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }

    return (unsigned int) -1;
}

static int __l716_rcvdeal_single(void *msg,size_t len)
{
    char           *data = (char *)msg;
    bool_t          isudp;
    int             fd;
    unsigned short  data_len = 0;
    int             ret;
    int             got = 0;

    isudp = data[7] == 'U' ? true : false;

    if (isudp) {
        data = strchr(strchr(data, ',') + 1, ',') + 1;
    } else {
        data = data + 12;
    }

    fd = *data - '0';

    /* skip fd and ',' */

    data += 2;

    data = strchr(data, ',');
    data++;

    /* now we are at data */

    while (data[data_len] != '\r') data_len++;

    data_len >>= 1;

    if (data_len > (CACHELEN - ring_buffer_datalen(&s_l716_cbs[fd]->l716_rcvring))) {
        return 0;
    }

    if (isudp) {
        ret = ring_buffer_write(&s_l716_cbs[fd]->l716_rcvring,
                                (unsigned char *)&data_len, sizeof(data_len));

        if (ret <= 0) {
            return ret;
        }
    }

    while (*data != '\r') {
        unsigned char ch;
        unsigned int  val1, val2;

        val1 = __atoi(data[0]);
        val2 = __atoi(data[1]);

        if ((val1 | val2) == (unsigned int) -1) {
            return -1;
        }

        ch = (unsigned char) ((val1 << 4) | val2);

        ret = ring_buffer_write(&s_l716_cbs[fd]->l716_rcvring, &ch, 1);

        if (ret <= 0) {
            return got == 0 ? -1 : got;
        }

        data += 2;
        got++;
    }

    return got;
}

static int __l716_rcvdeal(void *args,void *msg,size_t len)
{
    int ret = 0;

    if(osal_mutex_lock(g_iobufer_lock))
    {
        while (msg != NULL)
        {
            int r = __l716_rcvdeal_single(msg, len);

            if (r == -1)
            {
                break;
            }

            ret += r;

            msg = strstr(r * 2 + (char *)msg, RCVINDEX);
        }

        osal_mutex_unlock(g_iobufer_lock);
    }
    return ret == 0 ? -1 : ret;
}

static int __l716_socket(int domain, int type, int protocol)
{
    int i = 0;

    for (i = 1; i < MAX_NR_FDS; i++)
    {
        if (s_l716_cbs[i] == NULL)
        {
            break;
        }
    }

    if (i == MAX_NR_FDS)
        {
        return -1;
        }

    s_l716_cbs[i] = (l716_sock_cb_t *)malloc(sizeof(l716_sock_cb_t));

    if (s_l716_cbs[i] == NULL)
    {
        return -1;
    }

    memset(s_l716_cbs[i], 0, sizeof(l716_sock_cb_t));

	s_l716_cbs[i]->domain   = domain;
	s_l716_cbs[i]->type     = type;
	s_l716_cbs[i]->protocol = protocol;
	s_l716_cbs[i]->sockfd   = i;

    ring_buffer_init(&s_l716_cbs[i]->l716_rcvring, s_l716_cbs[i]->rcvbuf, CACHELEN, 0, 0);

	return i;
}

static int __l716_bind(int fd, const struct sockaddr *addr, int addrlen)
{
	// not implemented for client
    return 0;
}

static int __l716_listen(int fd, int backlog)
{
	// not implemented for client
    return 0;
}

static int __l716_accept(int fd, struct sockaddr *addr, int addrlen)
{
	// not implemented for client
    return 0;
}

static int __l716_connect(int fd, const struct sockaddr *addr, int addrlen)
{
    int ret = -1;
    const struct sockaddr_in *serv_addr;
    uint16_t                  remote_port;
    struct in_addr            remote_ip_int;
    char                     *remote_ip;
    int                       proto;

    if (addr == NULL)
    {
        return ret;
    }

    if (fd <= 0 || fd >= MAX_NR_FDS || s_l716_cbs[fd] == NULL)
    {
        return ret;
    }

    serv_addr = (const struct sockaddr_in *) addr;
    remote_port = ntohs(serv_addr->sin_port);
    remote_ip_int = serv_addr->sin_addr;
    remote_ip = inet_ntoa(remote_ip_int);

    proto = s_l716_cbs[fd]->type == SOCK_DGRAM ? 1 : 0;

    //
    // format:
    // AT+MIPOPEN=fd,port,"remote_ip",remote_port,proto
    //
    // proto: 0 - TCP, 1 - UDP
    //

    if(osal_mutex_lock(g_atcombo_lock)){
        __l716_atcmd_noindex("AT+MIPOPEN=");
        __l716_atcmd_int_noindex(fd);
        __l716_atcmd_noindex(",,\"");
        __l716_atcmd_noindex(remote_ip);
        __l716_atcmd_noindex("\",");
        __l716_atcmd_int_noindex(remote_port);
        __l716_atcmd_noindex(",");
        __l716_atcmd_int_noindex(proto);

        if (!__l716_atcmd_simple("\r\n", "MIPOPEN")) {
            ret = -1;
        }
        else{
            s_l716_cbs[fd]->connected = true;
            ret = 0;
        }
        osal_mutex_unlock(g_atcombo_lock);
    }
    return ret;
}

static int __l716_send(int fd, const void *buf, int len, int flags)
{
    int ret = -1;
    if (fd <= 0 || fd > MAX_NR_FDS) {
        return ret;
    }

    if(osal_mutex_lock(g_atcombo_lock)){

        __l716_atcmd_noindex("AT+MIPSEND=");
        __l716_atcmd_int_noindex (fd);
        __l716_atcmd_noindex(",");
        __l716_atcmd_int_noindex(len);
        __l716_atcmd_simple("\r\n", ">");

        ret = at_command((const void *)buf, len, "MIPSEND", NULL, 0, CMDTIMEOUT) >= 0 ? len : -1;

        osal_mutex_unlock(g_atcombo_lock);
    }

    return ret;
}

static int __l716_sendto(int fd, const void *msg, int len, int flags,
                         struct sockaddr *addr, int addrlen)
{
	if (!s_l716_cbs[fd]->connected) {
		__l716_connect(fd, addr, addrlen);
	}

    return __l716_send(fd, msg, len, flags);
}

static int __l716_recv(int fd, void *buf, size_t len, int flags)
{
	int          ret     = -1;
	unsigned int timeout = 0;

	timeout = s_l716_cbs[fd]->timeout;

	do {

	    if(osal_mutex_lock(g_iobufer_lock))
	    {
	        if (s_l716_cbs[fd]->type == SOCK_DGRAM) {
	            unsigned short data_len = 0;
	            ret = ring_buffer_read(&s_l716_cbs[fd]->l716_rcvring,
	                                   (unsigned char *)&data_len, sizeof(data_len));
	            ret = ring_buffer_read(&s_l716_cbs[fd]->l716_rcvring,
	                                   (unsigned char *)buf, data_len);

	        } else {
	            ret = ring_buffer_read(&s_l716_cbs[fd]->l716_rcvring, (unsigned char *)buf, len);
	        }

	        osal_mutex_unlock(g_iobufer_lock);
	    }

        if (ret > 0) {
            return ret;
        }

        if (timeout > 0) {
            osal_task_sleep(1);
        }
	} while (timeout-- > 0);

	return -1;
}

static int __l716_recvfrom(int fd, void *msg, int len, int flags, struct sockaddr *addr, socklen_t *addrlen)
{
    return __l716_recv(fd, msg, len, flags);
}

static int __l716_setsockopt(int fd, int level, int option, const void *option_value, int option_len)
{
	struct timeval *timedelay = (struct timeval *)option_value;
	s_l716_cbs[fd]->timeout = (timedelay->tv_sec * 1000) + (timedelay->tv_usec / 1000);
    return 0;
}

static int __l716_getsockopt(int fd, int level, int optname, const void *optval, int optlen)
{
	//not implemented
    return 0;
}

static int __l716_close(int fd)
{
    int ret = -1;

    if(osal_mutex_lock(g_atcombo_lock)){

        __l716_atcmd_noindex("AT+MIPCLOSE=");
        __l716_atcmd_int_noindex(fd);

        if (__l716_atcmd_simple("\r\n", "MIPCLOSE:")) {

            osal_free(s_l716_cbs[fd]);
            s_l716_cbs[fd] = NULL;
            ret = 0;
        }

        osal_mutex_unlock(g_atcombo_lock);
    }

    return ret;
}

static int __l716_shutdown(int fd, int how)
{
	return __l716_close(fd);
}

static int __l716_getsockname(int fd, struct sockaddr *addr,socklen_t *addrlen)
{
    //not implemented
    return 0;
}

static int __l716_getpeername(int fd, struct sockaddr *addr,socklen_t *addrlen)
{
    //not implemented
    return 0;
}

static struct hostent *__l716_gethostbyname(const char *name)
{
    char  resp[96];
	char *str;
    int   ipv4[4];

    static struct hostent hostent;
    static char           ipv4_ary[4];
    static char          *ipv4_lst[2] = { NULL, NULL };

    if (sscanf(name, "%d.%d.%d.%d", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]) != 4) {

        if(osal_mutex_lock(g_atcombo_lock)){

            __l716_atcmd_noindex("at +mping=1,\"");
            __l716_atcmd_noindex(name);

            if (!__l716_atcmd("\",1\r\n", "+MPING:", resp, 96)) {
                osal_mutex_unlock(g_atcombo_lock);
                return NULL;
            }
            else{
                osal_mutex_unlock(g_atcombo_lock);
            }
        }

        str = strchr(resp, '"');

        if (str == NULL) {
            return NULL;
        }

        str++;

        memset(&hostent, 0, sizeof(struct hostent));

        if (sscanf(name, "%d.%d.%d.%d", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]) <= 0) {
            return NULL;
        }
    }

    ipv4_ary[0] = ipv4[0];
    ipv4_ary[1] = ipv4[1];
    ipv4_ary[2] = ipv4[2];
    ipv4_ary[3] = ipv4[3];

    ipv4_lst[0] = ipv4_ary;

    hostent.h_addr_list = ipv4_lst;
    hostent.h_addrtype = AF_INET;
    hostent.h_length = 4;

    return &hostent;
}

static const tag_tcpip_ops s_tcpip_socket_ops =
{
    .socket        = (fn_sal_socket)        __l716_socket,
    .bind          = (fn_sal_bind)          __l716_bind,
    .listen        = (fn_sal_listen)        __l716_listen,
    .connect       = (fn_sal_connect)       __l716_connect,
    .accept        = (fn_sal_accept)        __l716_accept,
    .send          = (fn_sal_send)          __l716_send,
    .sendto        = (fn_sal_sendto)        __l716_sendto,
    .recv          = (fn_sal_recv)          __l716_recv,
    .recvfrom      = (fn_sal_recvfrom)      __l716_recvfrom,
    .setsockopt    = (fn_sal_setsockopt)    __l716_setsockopt,
    .getsockopt    = (fn_sal_getsockopt)    __l716_getsockopt,
    .shutdown      = (fn_sal_shutdown)      __l716_shutdown,
    .closesocket   = (fn_sal_closesocket)   __l716_close,
    .getpeername   = (fn_sal_getpeername)   __l716_getpeername,
    .getsockname   = (fn_sal_getsockname)   __l716_getsockname,
    .gethostbyname = (fn_sal_gethostbyname) __l716_gethostbyname,
};

static const tag_tcpip_domain s_tcpip_socket =
{
    .name = "l716 socket",
    .domain = AF_INET,
    .ops = &s_tcpip_socket_ops,
};

static bool_t _l716_joinnetwork(void)
{
    extern void atdevice_enable(void);
    
    do {
        atdevice_enable();
    } while (__l716_poll_cmd("AT\r\n", "OK", 1000, 10) != 0);

    if (__l716_poll_cmd("AT+CPIN?\r\n", "READY", 1000, 3) != 0) {
        return false;
    }

    if (__l716_poll_cmd("AT+CSQ\r\n", "CSQ:", 1000, 3) != 0) {
        return false;
    }

    if (__l716_poll_cmd("AT+CGREG?\r\n", "CGREG:", 1000, 3) != 0) {
        return false;
    }

    (void) __l716_atcmd_simple("ATE0\r\n", "OK");

    if (__l716_poll_cmd("AT+MIPCALL=1,\"ctnet\"\r\n", "+MIPCALL", 3000, 10) != 0) {
        return false;
    }

    if (__l716_poll_cmd("AT+MIPCALL?\r\n", "+MIPCALL: 1", 1000, 3) != 0) {
        return false;
    }

    at_debugclose();

    return true;
}

int link_tcpip_imp_init(void)
{
    int ret = -1;

    s_l716_cbs[0] = (l716_sock_cb_t *) -1;

    if(false == osal_mutex_create(&g_atcombo_lock))
    {
        printf(" l716 AT MUTEX_ERR\r\n");
        return ret;
    }

    if(false == osal_mutex_create(&g_iobufer_lock))
    {
        osal_mutex_del(g_atcombo_lock);
        printf(" l716 IO MUTEX_ERR\r\n");
        return ret;
    }

    at_oobregister("l716cn", RCVINDEX, strlen(RCVINDEX), __l716_rcvdeal, NULL);

    while (!_l716_joinnetwork()) {
        printf("Fail to enable network and try another time\r\n");
        osal_task_sleep(1000);
    }

   //reach here means everything is ok, we can go now
    ret = link_sal_install(&s_tcpip_socket);

    printf("sal:install socket %s\r\n", ret == 0 ? "success" : "failed");

    return ret;
}

