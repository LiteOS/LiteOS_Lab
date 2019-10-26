dir-$(CONFIG_STM32H7) = *
src-$(CONFIG_STM32H7) += soc_init.c

# source $(soc).mk if any
-include $(soc).mk