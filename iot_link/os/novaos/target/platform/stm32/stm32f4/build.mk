dir-$(CONFIG_STM32F4) = *
src-$(CONFIG_STM32F4) += soc_init.c

# source $(soc).mk if any
-include $(soc).mk