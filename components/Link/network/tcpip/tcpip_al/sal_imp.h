/*
 * sal_imp.h
 *
 *  Created on: 2019Äê4ÔÂ26ÈÕ
 *      Author: zhangqf
 */

#ifndef __SAL_IMP_H
#define __SAL_IMP_H


/**
 * @brief: all the function model,please refer to the
 *
 * */
typedef struct
{
    void* (*socket)(int domain, int type, int protocol);

    int (*bind)(void *sock,const void *addr,int addrlen);

    int (*listen)(void *sock,  int backlog);

    int (*accept)(void *sock,void *addr,int *addrlen);

    int (*connect)(void *sock, const void *addr,int addrlen);

    int (*getsockname)(void *sock, void *addr,int *addrlen);

    int (*getpeername)(void *sock, void *addr,int *addrlen);

    int (*getsockopt)(void *sock,, int level, int optname, void *optval, int *optlen);

    int (*setsockopt)(void *sock,, int level, int optname, const void *optval, int optlen);

    int (*recv)(void *sock,void *buf,int len,int flags);

    int (*recvfrom)(void *sock, void *mem, int len, int flags,
          void *from, int *fromlen);

    int (*send)(void *sock,const void *buf,int len,int flags);

    int (*sendto)(void *sock, const void *dataptr, int size, int flags,
        const void *to, int tolen);

    int (*shutdown)(void *sock, int how);

    int (*closesocket)(void *sock);

}tag_tcpip_ops;


typedef struct
{
    const char     *name;    ///< this member reserved now
    int             domain;  ///< this member to match the tcpip ops,likes AF_INET and so on
    tag_tcpip_ops  *ops;     ///< this member used to implement the user's operation
}tag_tcpip_domain;


/**
 * @brief: the tcpip stack should use this function to install it to the socket
 *         abstract layer, and the after the installation, the user could use
 *         socket abstract api or the BSD api as your need
 * @param[in]: domain, refer to the structure tag_tcpip_domain,remember that it must
 *             not changed after the installation
 * @return: 0 success while -1 failed
 * */
int tcpip_sal_install(const tag_tcpip_domain *domain);


#endif /* __SAL_IMP_H */
