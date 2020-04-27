################################################################################
# this is used for compile the user defined oc demo
################################################################################

#if you have defined multiple demos by yourself, modify this file to compile the chosen demo.

#example for lwm2m


	
	#example for oc_streetlight_template
	ifeq ($(CONFIG_USER_DEMO), "oc_streetlight_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_streetlight_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_streetlight_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_SC1/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_SC1}
		user_demo_defs = -D CONFIG_OC_STREELIGHT_DEMO_ENABLE=1
		
	endif

	#example for oc_streetlight_infrared_template
	ifeq ($(CONFIG_USER_DEMO), "oc_streetlight_infrared_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_streetlight_infrared_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_streetlight_infrared_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_SC1_Infrared/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_SC1_Infrared}
		user_demo_defs = -D oc_streetlight_infrared_template=1
		
	endif

	#example for oc_agriculture_template
	ifeq ($(CONFIG_USER_DEMO), "oc_agriculture_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_agriculture_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_agriculture_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_IA1/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_IA1}
		user_demo_defs = -D CONFIG_OC_ARRICULTURE_DEMO_ENABLE=1
		
	endif

	#example for oc_track_template
	ifeq ($(CONFIG_USER_DEMO), "oc_track_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_track_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_track_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_ST1/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_ST1}
		user_demo_defs = -D CONFIG_OC_TRACK_DEMO_ENABLE=1
		
	endif

	#example for oc_smoke_template
	ifeq ($(CONFIG_USER_DEMO), "oc_smoke_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_smoke_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_smoke_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_SF1/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_SF1}
		user_demo_defs = -D CONFIG_OC_SMOKE_DEMO_ENABLE=1
	endif

	#example for oc_manhole_cover_template
	ifeq ($(CONFIG_USER_DEMO), "oc_manhole_cover_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_manhole_cover_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_manhole_cover_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_SC2/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_SC2}
		user_demo_defs = -D CONFIG_OC_MANHOLE_COVER_DEMO_ENABLE=1
	endif

	#example for oc_infrared_template
	ifeq ($(CONFIG_USER_DEMO), "oc_infrared_template")	
		user_demo_src  = ${wildcard $(TARGET_DIR)/Demos/oc_infrared_template/*.c}
		user_demo_inc = -I $(TARGET_DIR)/Demos/oc_infrared_template
		user_hardware_src = ${wildcard $(TARGET_DIR)/Hardware/E53_IS1/*.c} 
		user_hardware_inc = -I ${wildcard $(TARGET_DIR)/Hardware/E53_IS1}
		user_demo_defs = -D CONFIG_OC_INFRARED_DEMO_ENABLE=1
		
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
