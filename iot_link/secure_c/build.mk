################################################################################
# this is used for compile the at modules
# make sure that this module depends on the driver module
# make sure driver module is enabled
################################################################################

ifeq ($(CONFIG_SECUREC_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/secure_c/src/*.c}     		
    C_INCLUDES += -I $(iot_link_root)/secure_c/include
endif


