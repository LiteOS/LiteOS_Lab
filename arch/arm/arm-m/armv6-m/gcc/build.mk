src-y                      += los_dispatch.c los_startup.S
src-$(LOSCFG_FLATFORM_EXC) += los_hw_exc.S
src-$(LOSCFG_SYSCALL)      += los_syscall.S