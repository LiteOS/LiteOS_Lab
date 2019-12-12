################################################################################
# this is used for compile the user defined oc demo
################################################################################

#if you have defined multiple demos by yourself, modify this file to compile the chosen demo.

#example for lwm2m

ifeq ($(CONFIG_DEMO_TYPE), "none")	
	
	#example for oc_streetlight_template
	ifeq ($(CONFIG_USER_DEMO), "oc_streetlight_template")

		ifeq ($(CONFIG_OC_LWM2M_TYPE), "boudica150_oc")
			user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_streetlight_template/oc_streetlight_template.c}
			user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_streetlight_template
		endif

		ifeq ($(CONFIG_OC_LWM2M_TYPE), "soft")
			user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_streetlight_template/oc_streetlight_template_Agent.c}
			user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_streetlight_template
		endif
		
			user_hardware_src = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_SC1/*.c} 
			user_hardware_inc = -I ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_SC1}
			user_demo_defs = -D CONFIG_OC_STREELIGHT_DEMO_ENABLE=1
		
	endif

	#example for oc_agriculture_template
	ifeq ($(CONFIG_USER_DEMO), "oc_agriculture_template")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_agriculture_template/*.c}
		user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_agriculture_template
		user_hardware_src = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_IA1/*.c} 
		user_hardware_inc = -I ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_IA1}
		user_demo_defs = -D CONFIG_OC_ARRICULTURE_DEMO_ENABLE=1
		
	endif

	#example for oc_track_template
	ifeq ($(CONFIG_USER_DEMO), "oc_track_template")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_track_template/*.c}
		user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_track_template
		user_hardware_src = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_ST1/*.c} 
		user_hardware_inc = -I ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_ST1}
		user_demo_defs = -D CONFIG_OC_TRACK_DEMO_ENABLE=1
		
	endif

	#example for oc_smoke_template
	ifeq ($(CONFIG_USER_DEMO), "oc_smoke_template")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_smoke_template/*.c}
		user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_smoke_template
		user_hardware_src = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_SF1/*.c} 
		user_hardware_inc = -I ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_SF1}
		user_demo_defs = -D CONFIG_OC_SMOKE_DEMO_ENABLE=1
	endif

	#example for hello world	 
	ifeq ($(CONFIG_USER_DEMO), "hello_world_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/hello_world_demo/*.c}
		user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/hello_world_demo
		user_hardware_src = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_SC1/*.c} 
		user_hardware_inc = -I ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Hardware/E53_SC1}
		
		user_demo_defs = -D CONFIG_HELLO_WORLD_ENABLE=1
	endif

	#example for cloud map 
	ifeq ($(CONFIG_USER_DEMO), "oc_cloud_map_demo")	
		user_demo_src  = ${wildcard $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_cloud_map_demo/*.c}
		user_demo_inc = -I $(TOP_DIR)/targets/GD32F303_BearPi/Demos/oc_cloud_map_demo
		user_demo_defs = -D CONFIG_OC_LWM2M_CLOUD_MAP_ENABLE=1
	endif

	C_SOURCES += $(user_demo_src)
	C_INCLUDES += $(user_demo_inc)
	C_SOURCES += $(user_hardware_src)
	C_INCLUDES += $(user_hardware_inc)
	C_DEFS += $(user_demo_defs)
endif