# 使用USER APP
user app是单独的可执行文件。内核通过内核提供的系统调用使用内核功能。每个app一个可执行文件。同时在系统中可以有多个app，这种支持叫做`多bin`。

这里以`nrf51dk`板为例子。这块板子的资源如下：

| ROM START  | ROM SIZE   | RAM START  | RAM SIZE   |
|------------|------------|------------|------------|
| 0x00000000 | 0x00040000 | 0x20000000 | 0x00008000 |

我们对这些资源做如下划分：

|            | ROM START  | ROM SIZE   | RAM START  | RAM SIZE   |
|------------|------------|------------|------------|------------|
| kernel     | 0x00000000 | 0x0003e000 | 0x20000000 | 0x00006000 |
| app#1      | 0x0003e000 | 0x00001000 | 0x20006000 | 0x00001000 |
| app#2      | 0x0003f000 | 0x00001000 | 0x20007000 | 0x00001000 |

接下来的步骤演示工程的创建过程

注意，本例中使用KEIL作为演示，gnuc，sesa（Segger Embedded Studio for ARM）都也是可以的。

## 创建内核工程
1. `prj -w kernel nrf51dk`创建工程
2. `cd kernel`进入工程目录
3. `prj -s CONFIG_SVC=y CONFIG_ROM_SIZE=0x0003e000 CONFIG_RAM_SIZE=0x00006000 CONFIG_KEIL=y`配置工程，也可以使用`prj -m`配置，配置完成后会发现KEIL工程已经生成好了。
4. 修改工程中的样板文件`main.c`添加以下内容：
    ```
    #include <config.h>
    #include <task.h>

    task_id app_task_1;
    task_id app_task_2;

    int main (void)
        {
        app_task_1 = task_spawn ("mpu task#1", 20, 0, 0x400,
                                 (int (*) (uintptr_t)) 0x3e001, 0);
        app_task_2 = task_spawn ("mpu task#2", 20, 0, 0x400,
                                 (int (*) (uintptr_t)) 0x3f001, 0);

        return 0;
        }
    ```
5. 编译工程并配置下载方式后下载

## 创建APP工程
### 创建APP#1
1. `prj -w app1 nrf51dk`创建工程
2. `cd app1`进入工程目录
3. `prj -s CONFIG_USER_APP=y CONFIG_ROM_START=0x0003e000 CONFIG_ROM_SIZE=0x00001000 CONFIG_RAM_START=0x20006000 CONFIG_RAM_SIZE=0x00001000 CONFIG_KEIL=y`配置为user app，并配置rom，ram区间以及配置keil工程，配置完成后keil工程就生成了。打开工程，将编译优化等级改为`-O1`，因为`-O0`编译器会添加很多断点指令，多bin调试不方便。
4. 打开工程并打开`main.c`，会看到以下内容：
    ```
    #include <kprintf.h>

    int main (void)
        {
        kprintf ("hello world\n");
        }
    ```
    将其改为如下内容：
    ```
    #include <task.h>
    #include <kprintf.h>

    int main (void)
        {
        while (1)
            {
            kprintf ("this is user_app#1\n");
            task_delay (64 * 2);
            }
        }
    ```
5. 编译并配置下载
### 创建APP#2
1. `prj -w app2 nrf51dk`创建工程
2. `cd app2`进入工程目录
3. `prj -s CONFIG_USER_APP=y CONFIG_ROM_START=0x0003f000 CONFIG_ROM_SIZE=0x00001000 CONFIG_RAM_START=0x20007000 CONFIG_RAM_SIZE=0x00001000 CONFIG_KEIL=y`配置为user app，并配置rom，ram区间以及配置keil工程，配置完成后keil工程就生成了。打开工程，将编译优化等级改为`-O1`，因为`-O0`编译器会添加很多断点指令，多bin调试不方便。
4. 打开工程并打开`main.c`，会看到以下内容：
    ```
    #include <kprintf.h>

    int main (void)
        {
        kprintf ("hello world\n");
        }
    ```
    将其改为如下内容：
    ```
    #include <task.h>
    #include <kprintf.h>

    int main (void)
        {
        task_delay (64);
        while (1)
            {
            kprintf ("this is user_app#2\n");
            task_delay (64 * 2);
            }
        }
    ```
5. 编译并配置下载
## 在`kernel`工程下启动调试，执行就会看到user程序跑起来。