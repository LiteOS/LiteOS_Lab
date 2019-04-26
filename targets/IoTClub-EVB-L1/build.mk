src-y    += Usr/*.c Apps/*.c Hardware/*.c DeviceDriver/NB/nb.c DeviceDriver/BH1750/BH1750.c DeviceDriver/DHT11/DHT11_BUS.c
cflags-g += -DSTM32L431xx -DUSE_HAL_DRIVER
inc      += DeviceDriver/* Hardware Apps Usr