dir-$(CONFIG_STM32L4) = *
src-$(CONFIG_STM32L4) += soc_init.c

# source $(soc).mk if any
-include $(soc).mk