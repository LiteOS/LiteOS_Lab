################################################################################
# this is used for compile the iotlink
################################################################################


OSAL_SOURCE  = ${wildcard $(iot_link_root)/oskernel/osal/*.c}
C_SOURCES += $(OSAL_SOURCE)
		
OSAL_INC = -I $(iot_link_root)/oskernel/osal
C_INCLUDES += $(OSAL_INC)

#now only suport liteos
OSAL_CONFIG_OS = liteos

ifeq ($(OSAL_CONFIG_OS), liteos)
	include $(iot_link_root)/oskernel/liteos/liteos_imp.mk
endif


