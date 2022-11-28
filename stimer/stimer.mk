################################################################################
# this is used for compile the soft timer module
# make sure that this module depends on the osal module
# make sure the osal module is enabled and some os is selected for the link
################################################################################

ifeq ($(CONFIG_STIMER_ENABLE),y)
	stimer_src  = ${wildcard $(iot_link_root)/shell/*.c} 
    C_SOURCES += $(stimer_src)	
    
    stimer_inc = -I $(iot_link_root)/stimer
    C_INCLUDES += $(stimer_inc)
	
	ifeq ($(CONFIG_STIMER_DEMO_ENABLE),y)

	    C_SOURCES +=  $(iot_link_root)/stimer/stimer_demo.c

	endif


endif
