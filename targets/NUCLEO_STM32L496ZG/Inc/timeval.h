#ifndef __TIMEVAL_H__
#define __TIMEVAL_H__

#ifdef __KEIL__
struct timeval {
    long    tv_sec;
    long    tv_usec;
};
#endif

#endif

