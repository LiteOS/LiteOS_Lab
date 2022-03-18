# 使用样例

1. [多个单独的APP工程（多bin）](./documents/using_user_app.md)
2. [ARM Cortex-M非特权任务](./documents/using_mpu.md)
3. TODO：内存探测，内存管理，HAL串口，HAL定时器，延迟任务。动态时钟。bsp模型。通用数据结构。

# 环境搭建

1. 使用Ubuntu或在windows上使用MSYS2以及`make`和`python3`（如果在windows下使用msys2的话，请在msys2中使用`pacman`命令安装），具体安装步骤此处略过

2. 安装编译器

   * ARM 
     1. 下载arm交叉编译工具链，[下载地址](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm "arm交叉编译 工具链")， 根据页面文字指示选择对应的版本（Cortex-A或Cortex-M）
     ![](./documents/pic/arm-gcc.png "")
     Cortex-M选`GNU-RM`，Cortex-A选`GNU-A`。
     2. 解压到某个位置然后将`gcc-arm-none-eabi-8-2018-q4-major-win32/bin`对应的目录添加到`PATH`。
3. 克隆本仓库
   1. `git clone https://github.com/LiteOS/nova`
   2. `cd nova`
   3. `git submodule update --init --recursive`
4. 编译
   * 进入仓库根目录执行`source env.bash`如下图所示：
     ![](./documents/pic/env.png "")
   * 建立工程，配置工程，需要使用`prj`命令，使用`prj -h`查看其使用帮助：
     ![](./documents/pic/prj-help.png "")
   * 在仓库根目录下新建一个目录用了存放工程，比如`proj`目录，然后进入此目录。
   * 进入这个目录，然后执行`prj -w <prj-name> <bsp-name>`创建工程：
     ![](./documents/pic/prj-create.png "")
   * 进入工程目录，如果有特殊需求，可以使用`prj -s CONFIG_XXX=y CONFIG_XXX=nnn`的方式配置工程：
     ![](./documents/pic/prj-conf.png "")
   * `prj -v CONFIG_XXX`命令可以查看参数当前的配置:
     ![](./documents/pic/prj-value.png "")
   * `prj -m`可以使用`menuconfig`配置参数（所有配置都可以使用此方式配置）：
        ![](./documents/pic/prj-menu.png "")
        **注意**:在使用menuconfig时，直接使用`esc`退出出然后选择保存即可。或者在手动用`S`命令保存是，不要更改默认的配置文件名。
   * 在创建工程的时候，可以直接使用`prj -w 52 nrf52dk -m`这种命令创建然后直接进入menuconfig进行工程配置
   * 配置完成后，执行make命令编译，可以使用`-j8`这种参数加快编译速度
   * 对于支持IDE的bsp，还可以配置IDE选项然后然后使用prj命令生成的IDE工程文件编译调试
     1. keil，对于希望使用keil的用户则可以使用下图的命令生成keil工程然后使用keil工具编译调试：
         ![](./documents/pic/keil-gen.png "")
         使用keil打开红框中的工程文件即可编译下载了
     2. SES，SES是国际领先的调试工具厂商Segger开发的一款IDE，全称为：`Segger Embedded Studio for ARM`。这里可以使用`prj -s CONFIG_SESA=y`命令或`prj -m`或者在创建工程时使用`prj -w prj_dir bsp_name -m`然后配置SESA选项开启SES编译支持，当配置完成工程文件会自动创建（这里以`prj -s CONFIG_SESA=y`命令作为例子，另外两种方式结果类似）：
         ![](./documents/pic/sesa-gen.png "")
         使用SES打开红框中的工程文件即可编译下载了
     3. 注意：一旦配置IDE支持，重新配置工程选项都会自动更新IDE工程文件。
