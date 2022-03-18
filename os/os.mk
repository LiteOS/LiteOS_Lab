################################################################################
# this is used for compile the iotlink
################################################################################

C_SOURCES  += ${wildcard $(iot_link_root)/os/osal/*.c}		
C_INCLUDES += -I $(iot_link_root)/os/osal

#now support "liteos" and "linux" "macos",please select one you like to use
ifeq ($(CONFIG_LITEOS_ENABLE), y)
	include $(iot_link_root)/os/liteos/liteos_imp.mk
else ifeq ($(CONFIG_LINUXOS_ENABLE), y)
	include $(iot_link_root)/os/linux/linux_imp.mk
else ifeq ($(CONFIG_MACOS_ENABLE), y)
	include $(iot_link_root)/os/macos/macos_imp.mk	
else ifeq ($(CONFIG_NOVAOS_ENABLE), y)
	include $(iot_link_root)/os/novaos/novaos_imp.mk		
endif


