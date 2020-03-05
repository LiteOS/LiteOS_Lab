################################################################################
# this is used for compile the service module
################################################################################

ifeq ($(CONFIG_SERVICE_TYPE_CNCS),y)
    C_SOURCES += ${wildcard $(iot_link_root)/service/cncs_service/*.c}
    
endif

C_INCLUDES += -I $(iot_link_root)/service/cncs_service/
C_DEFS     += -D CONFIG_CNCS_SERVICE_ENABLE=1