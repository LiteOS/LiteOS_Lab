inc-g-y                        += h/
src-prefix                     := src/
src-$(CONFIG_TESTER)           += tester.c cmockery.c
src-$(CONFIG_MUTEX_CASE)       += test_mutex.c
src-$(CONFIG_SEM_CASE)         += test_sem.c
src-$(CONFIG_EVENT_CASE)       += test_event.c
src-$(CONFIG_MQ_CASE)          += test_mq.c
src-$(CONFIG_TIMER_CASE)       += test_timer.c
src-$(CONFIG_TASK_CASE)        += test_task.c
src-$(CONFIG_HOOK_CASE)        += test_hook.c
src-$(CONFIG_TLS_CASE)         += test_tls.c
src-$(CONFIG_DEFER_CASE)       += test_defer.c
src-$(CONFIG_MEMTRY_CASE)      += test_memtry.c
src-$(CONFIG_MEMPOOL_CASE)     += test_mempool.c
src-$(CONFIG_HEAP_CASE)        += test_heap.c

# for cmockery
cflags-l-$(CONFIG_KEIL)        += -Wno-switch-enum -Wno-covered-switch-default -Wno-missing-noreturn -Wno-conversion
cflags-l-$(CONFIG_KEIL)        += -Wno-newline-eof -Wno-undef -Wno-cast-qual -Wno-shadow -Wno-sign-compare
cflags-l-$(CONFIG_KEIL)        += -Wno-gnu-empty-initializer -Wno-unknown-warning-option
cflags-l-$(CONFIG_KEIL)        += -Wno-gnu-zero-variadic-macro-arguments
cflags-l-$(CONFIG_KEIL)        += -Wno-implicit-function-declaration -Wno-strict-prototypes -Wno-disabled-macro-expansion
