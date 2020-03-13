################################################################################
# this is used for compile the service module
################################################################################

C_SOURCES += ${wildcard $(iot_link_root)/service/oc_service/*.c}
C_INCLUDES += -I $(iot_link_root)/service/oc_service/
C_DEFS     += -D CONFIG_OCEAN_SERVICE_ENABLE=1