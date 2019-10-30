src-prefix               := src/
src-y                    += class.c critical.c defer.c mutex.c sem.c task.c tick.c obj.c
src-$(CONFIG_IPC_EVENT)  += event.c
src-$(CONFIG_IPC_MQ)     += msg_queue.c
src-$(CONFIG_SOFT_TIMER) += timer.c
inc-g-y                  += h/