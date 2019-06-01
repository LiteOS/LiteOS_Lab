################################################################################
# this is used for compile the iotlink
################################################################################

#now support liteos and linux macos

ifeq ($(cfg_os_type), liteos)
	include $(iot_link_root)/os/os_imp/liteos/liteos_imp.mk
else ifeq ($(cfg_os_type), linux)
	include $(iot_link_root)/os/os_imp/linux/linux_imp.mk
else ifeq ($(cfg_os_type), macos)
	include $(iot_link_root)/os/os_imp/macos/macos_imp.mk	
else
	
endif





