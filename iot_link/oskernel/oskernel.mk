################################################################################
# this is used for compile the iotlink
################################################################################


OSAL_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/oskernel/osal/*.c}
C_SOURCES += $(OSAL_SOURCE)
		
OSAL_INC = -I $(TOP_DIR)/iot_link/oskernel/osal
C_INCLUDES += $(OSAL_INC)

#now only suport liteos
OSAL_CONFIG_OS = liteos

ifeq ($(OSAL_CONFIG_OS), liteos)
	include $(TOP_DIR)/iot_link/oskernel/liteos/liteos_imp.mk
endif


