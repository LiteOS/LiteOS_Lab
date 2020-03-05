################################################################################
# this is used for compile the service module
################################################################################

ifeq ($(CONFIG_SERVICE_ENABLE),y)
	C_SOURCES  +=  ${wildcard $(iot_link_root)/service/*.c}

	C_INCLUDES += -I$(iot_link_root)/service

	C_DEFS     += -D CONFIG_SERVICE_ENABLE=1

	## oc service	
	ifeq ($(CONFIG_SERVICE_TYPE),"oceanconnect")
	    include $(iot_link_root)/service/oc_service/oc_service.mk
	endif

	## cncs service
	ifeq ($(CONFIG_SERVICE_TYPE_CNCS),y)
	    include $(iot_link_root)/service/cncs_service/cncs_service.mk
	endif
endif

ifeq ($(CONFIG_LOCATION_ENABLE),y)
	C_SOURCES  +=  ${wildcard $(iot_link_root)/service/location/*.c}

	C_INCLUDES += -I$(iot_link_root)/service/location

	C_DEFS     += -D CONFIG_LOCATION_ENABLE=1
endif