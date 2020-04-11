################################################################################
# this is used for compile the user defined oc demo
################################################################################

#if you have defined multiple demos by yourself, modify this file to compile the chosen demo.

#example for lwm2m
CONFIG_USER_DEMO := "oc_streetlight_template"

ifeq ($(CONFIG_USER_DEMO), "oc_streetlight_template")	
	
	
endif

C_SOURCES += $(user_demo_src)
C_INCLUDES += $(user_demo_inc)
C_DEFS += $(user_demo_defs)