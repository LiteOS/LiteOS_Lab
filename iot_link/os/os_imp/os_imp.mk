################################################################################
# this is used for compile the iotlink
################################################################################

#now only suport liteos
cfg_os_type = liteos

ifeq ($(cfg_os_type), liteos)
	include $(iot_link_root)/os/os_imp/liteos/liteos_imp.mk
endif


