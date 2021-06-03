################################################################################
# this is used for compile the user defined oc demo
################################################################################

#if you have defined multiple demos by yourself, modify this file to compile the chosen demo.

#example for lwm2m
#CONFIG_USER_DEMO="oc_streetlight_template"


	#example for oc_streetlight_template
	ifeq ($(CONFIG_USER_DEMO), "oc_streetlight_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_streetlight_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_streetlight_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/IoTBox/BH1750.c \
		$(TARGET_DIR)/Hardware/IoTBox/LIGHT.c \
		$(TARGET_DIR)/Hardware/IoTBox/BEEP.c \
		$(TARGET_DIR)/Hardware/DEMOS/BS_SC_DEMO.c}  
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/IoTBox}
		user_demo_defs = -D CONFIG_OC_LWM2M_DEMO_ENABLE=1
		
	endif

	#example for oc_agriculture_template
	ifeq ($(CONFIG_USER_DEMO), "oc_agriculture_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_agriculture_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_agriculture_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/IoTBox/BH1750.c \
		$(TARGET_DIR)/Hardware/IoTBox/SHT30.c \
		$(TARGET_DIR)/Hardware/IoTBox/BEEP.c \
		$(TARGET_DIR)/Hardware/IoTBox/LIGHT.c \
		$(TARGET_DIR)/Hardware/IoTBox/MOTOR.c\
		$(TARGET_DIR)/Hardware/IoTBox/LED.c \
		$(TARGET_DIR)/Hardware/DEMOS/BS_IA_DEMO.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/IoTBox}
		user_demo_defs = -D CONFIG_OC_LWM2M_DEMO_ENABLE=1
		
	endif

	#example for oc_manhole_cover_template
	ifeq ($(CONFIG_USER_DEMO), "oc_manhole_cover_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_manhole_cover_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_manhole_cover_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/IoTBox/rsrvd_interface/E53_SC2.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/IoTBox/rsrvd_interface}
		user_demo_defs = -D CONFIG_OC_MANHOLE_COVER_DEMO_ENABLE=1
	endif

	#example for oc_infrared_template
	ifeq ($(CONFIG_USER_DEMO), "oc_infrared_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_infrared_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_infrared_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/IoTBox/rsrvd_interface/E53_IS1.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/IoTBox/rsrvd_interface}
		user_demo_defs = -D CONFIG_OC_INFRARED_DEMO_ENABLE=1
		
	endif

	#example for osal_task_demo
    ifeq ($(CONFIG_USER_DEMO), "osal_task_demo")    
        user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/osal_kernel_demo/osal_task_demo.c}
        user_demo_defs = -D CONFIG_OSAL_TASK_DEMO_ENABLE=1
    endif

#example for osal_semp_demo
    ifeq ($(CONFIG_USER_DEMO), "osal_semp_demo")    
        user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/osal_kernel_demo/osal_semp_demo.c}
        user_demo_defs = -D CONFIG_OSAL_SEMP_DEMO_ENABLE=1
    endif

#example for osal_mutex_demo
    ifeq ($(CONFIG_USER_DEMO), "osal_mutex_demo")    
        user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/osal_kernel_demo/osal_mutex_demo.c}
        user_demo_defs = -D CONFIG_OSAL_MUTEX_DEMO_ENABLE=1
    endif

#example for osal_mem_demo
    ifeq ($(CONFIG_USER_DEMO), "osal_mem_demo")    
        user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/osal_kernel_demo/osal_mem_demo.c}
        user_demo_defs = -D CONFIG_OSAL_MUTEX_DEMO_ENABLE=1s
    endif

	#example for oc_track_template
	ifeq ($(CONFIG_USER_DEMO), "oc_track_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_track_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_track_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/Demos/BS_ST_DEMO.c \
		$(TARGET_DIR)/Hardware/IoTBox/BEEP.c \
		$(TARGET_DIR)/Hardware/IoTBox/GPS.c \
		$(TARGET_DIR)/Hardware/IoTBox/LED.c}
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/IoTBox}
		user_demo_defs = -D CONFIG_OC_LWM2M_DEMO_ENABLE=1
		
	endif

	#example for oc_smoke_template
	ifeq ($(CONFIG_USER_DEMO), "oc_smoke_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_smoke_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_smoke_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/Demos/BS_SF_DEMO.c \
		$(TARGET_DIR)/Hardware/IoTBox/LED.c \
		$(TARGET_DIR)/Hardware/IoTBox/SmokeSensor.c \
		$(TARGET_DIR)/Hardware/IoTBox/BEEP.c \
		$(TARGET_DIR)/Hardware/IoTBox/LIGHT.c \} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/IoTBox}
		user_demo_defs = -D CONFIG_OC_LWM2M_DEMO_ENABLE=1
	endif

	#example for hello world	 
	ifeq ($(CONFIG_USER_DEMO), "hello_world_demo")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/hello_world_demo/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/hello_world_demo
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif

	#example for cloud map 
	ifeq ($(CONFIG_USER_DEMO), "oc_cloud_map_demo")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_cloud_map_demo/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_cloud_map_demo
		user_demo_defs = -D CONFIG_OC_LWM2M_CLOUD_MAP_ENABLE=1
	endif

	C_SOURCES += $(user_demo_src)
	C_INCLUDES += $(user_demo_inc)
	C_SOURCES += $(user_hardware_src)
	C_INCLUDES += $(user_hardware_inc)
	C_DEFS += $(user_demo_defs)
