# Huawei Agent Tiny SDK
# SW4STM32(AC6) GCC 开发环境搭建指南

## 目 录
<!-- TOC -->

- [1 环境准备](#1)
    - [1.1 安装GCC 编译器](#1.1)
    - [1.2 安装jdk 运行环境](#1.2)
    - [1.3 安装make 工具](#1.3)
    - [1.4 安装Jlink 驱动程序](#1.4)
    - [1.5 安装Eclipse IDE 环境](#1.5)
    - [1.6 安装AC6](#1.6)
- [2 获取LiteOS最新源码](#2) 
- [3 工程创建与配置](#3)
    - [3.1 创建工程](#3.1)
    - [3.2 导入IoT link工程源码](#3.2)
    - [3.3 配置工程](#3.3)
    - [3.4 配置调试器](#3.4)
<!-- /TOC -->
<h1 id="1">1.环境准备</h1>
参考网址：https://github.com/LiteOS/LiteOS_Lab/blob/iot_link/doc/Huawei_IoT_Link_SDK_IDE_Eclipse_Guide.md

<h2 id="1.1">1.1 安装GCC 编译器</h2>
首先需要安装GCC编译器” GNU Arm Embedded Toolchain”，安装完毕务必将添加环境变量“Add path to environment variable”勾选上。 

下载地址: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads

<h2 id="1.2">1.2 安装jdk 运行环境</h2>
安装好jdk后，新建环境变量，并添加path路径。安装后可以windows+R，输入cmd进入terminal，输入java -version，查看版本以及安装成功与否。

下载地址： http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html

<h2 id="1.3">1.3 安装make 工具</h2>
Make 工具用于build 工程，使用Makefile 编译工程。也可根据需要自行安装所需版本。

版本：gnu-mcu-eclipse-build-tools-2.9-20170629-1013-win64-setup.exe

下载地址: https://github.com/gnu-mcu-eclipse/windows-build-tools/releases/tag/v2.9-20170629-1013

<h2 id="1.4">1.4 安装Jlink 驱动程序</h2>
版本：JLink_Windows_V632e.exe  下载地址如下：https://www.segger.com/downloads/jlink/#JLinkSoftwareAndDocumentationPack 

<h2 id="1.5">1.5 安装Eclipse IDE 环境</h2>

下载地址: https://github.com/gnu-mcu-eclipse/org.eclipse.epp.packages/releases

将下载的eclipse 直接解压即可完成安装。解压后，打开eclipse 目录，如图双击eclipse.exe 即可运行IDE 环境,建议将该应用程序创建桌面快捷方式，这样方便后续直接在桌面打开IDE环境。

<h2 id="1.6">1.6 安装AC6环境</h2>
下载最新版本v2.9，即可直接官网https://www.ac6-tools.com/downloads/SW4STM32/install_sw4stm32_win_32bits-v2.9.exe

<h1 id="2">2.获取LiteOS源码</h1>
下载LiteOS最新源码，并解压到指定文件夹。下载地址：https://github.com/LiteOS/LiteOS_Lab

<h1 id="2">3.工程创建与配置</h1>

<h2 id="3.1">3.1 创建工程</h2>
在软件中，点击File → New → C project，开始创建工程，具体如下所示。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_01.png)

自定义工程名字，选择创建空项目，以及Ac6 STM32 MCU GCC。

![](./meta/IoT_link/ide/SW4STM32(AC6)_GCC/chen_AC6_02.png)

之后选择开发板，选择对应系列到具体开发板，具体如下所示，可以看到同一系列的一类开发板。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_03.png)

如果找不到，点击新建开发板。本文使用的是STM32F429IGTx的野火开发板，采用创建新开发板方式。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_04.png)

根据需要创建开发板并选择具体的调试接口，本文使用JTAG，注意所选ＭＣＵ名称。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_05.png)

接下来点击ＯＫ，可以看见开发板的详细信息，包括系列名，以及RAM、ROM的起始地址以及具体大小。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_06.png)

由于后续将采用LiteOS的启动文件，此处勾选不生成启动文件。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_07.png)

生成的具体目录情况如下所示，除了Includes文件夹，其余文件夹为空。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_08.png)



<h2 id="3.2">3.2 导入IoT link工程源码</h2>
之后在Src文件夹下，新建目录，具体操作如下所示，命名为Huawei_Lab_IoT_link，同时将其链接到LiteOS源码所在文件夹。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_09.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_10.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_11.png)

点击Finish完成创建，打开Src下的Huawei_Lab_IoT_link文件夹，可以看见完整的工程目录。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_12.png)

<h2 id="3.3">3.3 配置工程</h2>
之后点击工具栏中Project下的Properties选项，选择C/C++ build，选择修改Build directory，将makefile generation 勾选取消，选择已有的Makefile文件，选择为LiteOS源码下D:\LiteOS\Huawei_IoT_link\LiteOS_Lab-iot_link\targets\STM32F429IGTx_FIRE\GCC文件（上述为我的链接文件路径）。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_13.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_14.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_15.png)

之后clean项目，然后选择工具栏的project选项中的build project当出现如下所示的内容时，说明make成功。
![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_16.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_17.png)

<h2 id="3.4">3.4 配置调试器</h2>

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_18.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_19.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_20.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_21.png)

接下来新建Jlink调试器，，进行如图所示的配置，选择debugger选项中添加设备名称为stm32f429IG。勾选Disable auto build。

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_22.png)

![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_23.png)

之后在Executable中选择，所安装的GNU目录项的bin文件夹下的arm-none-eabi-gdb.exe文件。之后在Common选项下选择Shared file。之后编译项目即可。
![](./meta/IoT_Link/ide/SW4STM32(AC6)_GCC/chen_AC6_24.png)


