#ifndef __HOST_IRQ_H__
#define __HOST_IRQ_H__

#include <stdbool.h>

unsigned int int_cnt = 0;

static bool int_context_arch (void)
    {
    return false;
    }

#endif /* __HOST_IRQ_H__ */

