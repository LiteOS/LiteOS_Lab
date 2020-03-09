################################################################################
# this is used for compile the iotlink
################################################################################

#we will add more arch here,now support the armv7-m armv6-m and riscv32
NOVA_ROOT  = $(iot_link_root)/os/novaos
C_SOURCES += \
            $(NOVA_ROOT)/target/phys_mem.c  \
            $(NOVA_ROOT)/utils/src/bug.c  \
            $(NOVA_ROOT)/utils/src/hook_table.c  \
            $(NOVA_ROOT)/utils/src/rbtree.c  \
            $(NOVA_ROOT)/utils/src/ring.c  \
            $(NOVA_ROOT)/utils/src/xprintf.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/context.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/cpu.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/exception.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/handlers.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/nvic.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/systick.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/trace.c  \
            $(NOVA_ROOT)/arch/aarch-m/src/vectors.c  \
            $(NOVA_ROOT)/components/cmder/src/cmder.c  \
            $(NOVA_ROOT)/components/cmder/src/cmder_mem.c  \
            $(NOVA_ROOT)/components/cmder/src/cmder_uart.c  \
            $(NOVA_ROOT)/components/kprintf/src/kprintf.c  \
            ${wildcard $(NOVA_ROOT)/components/fs/src/*.c} \
            $(NOVA_ROOT)/core/hal/src/hal_int.c  \
            $(NOVA_ROOT)/core/hal/src/hal_timer.c  \
            $(NOVA_ROOT)/core/hal/src/hal_uart.c  \
            ${wildcard $(NOVA_ROOT)/core/kernel/src/*.c}  \
            ${wildcard $(NOVA_ROOT)/core/mem/src/*.c}  \
            $(NOVA_ROOT)/core/services/src/errno.c  \
            $(NOVA_ROOT)/core/services/src/nova_init.c



novaos_inc = \
             -I $(NOVA_ROOT)/include \
             -I $(NOVA_ROOT)/arch/h \
             -I $(NOVA_ROOT)/utils/h \
             -I $(NOVA_ROOT)/arch/aarch-m/h \
             -I $(NOVA_ROOT)/arch/aarch-m/h/cmsis \
             -I $(NOVA_ROOT)/components/cmder/h \
             -I $(NOVA_ROOT)/components/kprintf/h \
             -I $(NOVA_ROOT)/components/fs/h \
             -I $(NOVA_ROOT)/core/hal/h \
             -I $(NOVA_ROOT)/core/kernel/h \
             -I $(NOVA_ROOT)/core/mem/h \
             -I $(NOVA_ROOT)/core/services/h

C_INCLUDES += $(novaos_inc)


C_DEFS += -D CONFIG_NOVAOS_ENABLE=1 -D STM32F429xx

C_SOURCES += $(NOVA_ROOT)/novaos_imp.c
C_INCLUDES += -I $(NOVA_ROOT)
