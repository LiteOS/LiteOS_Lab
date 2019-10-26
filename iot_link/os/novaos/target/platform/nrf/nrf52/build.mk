dir-$(CONFIG_NRF52) = *
src-$(CONFIG_NRF52) += soc_init.c

# source $(soc).mk if any
-include $(soc).mk