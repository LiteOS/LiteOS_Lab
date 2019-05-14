################################################################################
# this is used for compile the iotlink
################################################################################

ARCH_SRC =  \
        ${wildcard $(TOP_DIR)/arch/arm/arm-m/src/*.c} \
        ${wildcard $(TOP_DIR)/arch/arm/arm-m/armv7-m/*.c} \
        ${wildcard $(TOP_DIR)/arch/arm/arm-m/armv7-m/gcc/*.c}
        C_SOURCES += $(ARCH_SRC)
ARCH_INC = \
        -I $(TOP_DIR)/arch/arm/arm-m/include \
        -I $(TOP_DIR)/arch/arm/arm-m/armv7-m
        C_INCLUDES += $(ARCH_INC)         
UTIL_SRC = \
        $(TOP_DIR)/utils/rbtree.c
        C_SOURCES += $(UTIL_SRC)

KERNEL_SRC =  \
        ${wildcard $(TOP_DIR)/kernel/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/core/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/ipc/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/heap/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/membox/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/common/los_slab.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/common/los_slabmem.c} \
        ${wildcard $(TOP_DIR)/kernel/base/mem/common/los_memcheck.c} \
        ${wildcard $(TOP_DIR)/kernel/base/misc/*.c} \
        ${wildcard $(TOP_DIR)/kernel/base/om/*.c} \
        ${wildcard $(TOP_DIR)/kernel/extended/tickless/*.c}
        C_SOURCES += $(KERNEL_SRC)
KERNEL_INC = \
        -I $(TOP_DIR)/kernel/include \
        -I $(TOP_DIR)/kernel/base/include \
        -I $(TOP_DIR)/kernel/extended/include
        C_INCLUDES += $(KERNEL_INC)        
        
CMSIS_SRC =  \
        ${wildcard $(TOP_DIR)/osdepends/liteos/cmsis/*.c}
        C_SOURCES += $(CMSIS_SRC)
CMSIS_INC = \
        -I $(TOP_DIR)/arch/arm/arm-m/include \
        -I $(TOP_DIR)/arch/arm/common/cmsis
        C_INCLUDES += $(CMSIS_INC)
        
LITEOS_CMSIS_SRC = \
        -I $(TOP_DIR)/osdepends/liteos/cmsis
        C_INCLUDES += $(LITEOS_CMSIS_SRC)

LITEOS_IMP_SOURCE  = ${wildcard $(iot_link_root)/oskernel/liteos/*.c}
C_SOURCES += $(LITEOS_IMP_SOURCE)
		
LITEOS_IMP_INC = -I $(iot_link_root)/oskernel/liteos
C_INCLUDES += $(LITEOS_IMP_INC)

