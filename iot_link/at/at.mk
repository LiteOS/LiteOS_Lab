################################################################################
# this is used for compile the at modules
# make sure that this module depends on the driver module
# make sure driver module is enabled
################################################################################

ifeq ($(cfg_at_enable),yes)
    AT_MODULE_SRC = ${wildcard $(iot_link_root)/at/*.c} 
    C_SOURCES += $(AT_MODULE_SRC)
		
    AT_MODULE_INC = -I $(iot_link_root)/at
    C_INCLUDES += $(AT_MODULE_INC)

    AT_MODULE_DEF = -D cfg_at_enable=1
    C_DEFS += $(AT_MODULE_DEF)
endif

