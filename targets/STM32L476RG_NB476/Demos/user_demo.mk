################################################################################
# this is used for compile the user defined oc demo
################################################################################

#if you have defined multiple demos by yourself, modify this file to compile the chosen demo.

#example for LiteOS func


#example for report temperature	 
ifeq ($(CONFIG_USER_DEMO), "use_coap_demo")	
	user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/use_coap_demo/*.c}
	user_demo_defs = -D CONFIG_USE_COAP_ENABLE=1
endif


C_SOURCES += $(user_demo_src)
C_INCLUDES += $(user_demo_inc)
C_DEFS += $(user_demo_defs)
