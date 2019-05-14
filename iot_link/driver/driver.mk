################################################################################
# this is used for compile the at modules
# make sure that this module depends on the driver module
# make sure driver module is enabled
################################################################################

DRIVER_MODULE_SRC  = ${wildcard $(iot_link_root)/driver/*.c} 
C_SOURCES += $(DRIVER_MODULE_SRC)	
		
DRIVER_MODULE_INC = -I $(iot_link_root)/driver
C_INCLUDES += $(DRIVER_MODULE_INC)


