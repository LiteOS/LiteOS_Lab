################################################################################
# this is used for compile the user defined oc demo
################################################################################

#if you have defined multiple demos by yourself, modify this file to compile the chosen demo.

#example for LiteOS func

ifeq ($(CONFIG_DEMO_TYPE), "none")	

	#example for hello world	 
	ifeq ($(CONFIG_USER_DEMO), "hello_world_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi_OS_Func/Demos/hello_world_demo/*.c}
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif
	
	#example for osal_task_demo	 
	ifeq ($(CONFIG_USER_DEMO), "osal_task_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi_OS_Func/Demos/osal_task_demo/*.c}
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif

	#example for osal_mem_demo	 
	ifeq ($(CONFIG_USER_DEMO), "osal_mem_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi_OS_Func/Demos/osal_mem_demo/*.c}
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif

	#example for osal_semp_demo	 
	ifeq ($(CONFIG_USER_DEMO), "osal_semp_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi_OS_Func/Demos/osal_semp_demo/*.c}
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif

	#example for osal_mutex_demo 
	ifeq ($(CONFIG_USER_DEMO), "osal_mutex_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/STM32L431_BearPi_OS_Func/Demos/osal_mutex_demo/*.c}
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif

	
	C_SOURCES += $(user_demo_src)
	C_INCLUDES += $(user_demo_inc)
	C_DEFS += $(user_demo_defs)
endif