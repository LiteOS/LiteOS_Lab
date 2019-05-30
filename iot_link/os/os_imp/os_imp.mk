################################################################################
# this is used for compile the iotlink
################################################################################

#now support liteos and linux
#cfg_os_type := linux

ifeq ($(cfg_os_type), liteos)
	include $(iot_link_root)/os/os_imp/liteos/liteos_imp.mk
else ifeq ($(cfg_os_type), linux)
	include $(iot_link_root)/os/os_imp/linux/linux_imp.mk
endif

