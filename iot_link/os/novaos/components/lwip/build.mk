src-$(CONFIG_LWIP)      += port/src/*.c                     \
                           lwip-2.1.2/src/api/*.c           \
                           lwip-2.1.2/src/core/*.c          \
                           lwip-2.1.2/src/core/ipv4/*.c     \
                           lwip-2.1.2/src/netif/ethernet.c

inc-g-$(CONFIG_LWIP)    += lwip-2.1.2/src/include           \
                           port/h

cflags-l-$(CONFIG_LWIP) += -Wno-missing-noreturn -Wno-covered-switch-default -Wno-sign-conversion -Wno-sign-conversion -Wno-conversion -Wno-switch-enum
