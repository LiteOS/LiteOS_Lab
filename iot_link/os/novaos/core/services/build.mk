src-prefix            := src
src-y                 += errno.c init.c
src-$(CONFIG_SYSCALL) += syscall.c
src-$(CONFIG_PROFILE) += profile.c
inc-g-y               += h