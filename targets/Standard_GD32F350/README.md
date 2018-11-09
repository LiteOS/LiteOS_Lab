## 使用说明

本工程为LiteOS移植GD32F350RBT6开发板的示例工程，通过创建任务控制LED灯闪烁。GD32F350RBT6拥有16k SRAM，128k FLASH。

由于授权原因，此工程并未上传GD32F3x0的官方库文件。若要正常运行，请：

1. 打开keil工程。keil会自动下载所需的官方库文件到 ./Keil_v5/ARM/PACK 下。
2. 在/tatget/Standard_GD32F350目录下新建文件夹Drivers/GD32F3x0_standard_peripheral/Include 与 Drivers/GD32F3x0_standard_peripheral/Source。
3. 将keil下载的官方库文件复制到工程路径下。/Keil_v5/ARM/PACK/GigaDevice/GD32F3x0_DFP/1.0.0/Device/Firmware/Peripherals/src中所有文件复制到上一步新建的Drivers/GD32F3x0_standard_peripheral/Source中，/Keil_v5/ARM/PACK/GigaDevice/GD32F3x0_DFP/1.0.0/Device/Firmware/Peripherals/inc中所有文件复制到Drivers/GD32F3x0_standard_peripheral/Include中。

完成上述步骤后，即可正常编译工程。GD32F350RBT6板载正版GDLINK，供电，串口打印，下载调试均可通过开发板上micro USB接口完成。