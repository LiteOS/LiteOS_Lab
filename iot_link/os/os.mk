################################################################################
# this is used for compile the iotlink
################################################################################

osal_src  = ${wildcard $(iot_link_root)/os/osal/*.c}
C_SOURCES += $(osal_src)
		
osal_inc = -I $(iot_link_root)/os/osal
C_INCLUDES += $(osal_inc)

#now support "liteos" and "linux" "macos",please select one you like to use
ifeq ($(CONFIG_OS_TYPE), "liteos")
	include $(iot_link_root)/os/liteos/liteos_imp.mk
else ifeq ($(CONFIG_OS_TYPE), "linux")
	include $(iot_link_root)/os/linux/linux_imp.mk
else ifeq ($(CONFIG_OS_TYPE), "macos")
	include $(iot_link_root)/os/macos/macos_imp.mk	
else ifeq ($(CONFIG_OS_TYPE), "novaos")
	include $(iot_link_root)/os/novaos/novaos_imp.mk		
endif


