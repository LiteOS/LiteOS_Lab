# 使用ARM MPU
ARM MPU提供对地址区间的保护配合user app（多bin）效果最佳。也可以不使用多bin方案，不过需要您通过链接脚本控制app的段区域。

## MPU模块提供的权限设置
| mem attribute     | 特权模式权限   | 非特权模式权限 |
|-------------------|----------------|----------------|
| `MPU_ATTR_______` | 无             | 无             |
| `MPU_ATTR_RW____` | 可读可写       | 无             |
| `MPU_ATTR_RW_R__` | 可读可写       | 可读           |
| `MPU_ATTR_RW_RW_` | 可读可写       | 可读可写       |
| `MPU_ATTR_R_____` | 可读           | 无             |
| `MPU_ATTR_R__R__` | 可读           | 可读           |
| `MPU_ATTR_RWX___` | 可读可写可执行 | 无             |
| `MPU_ATTR_RWXR_X` | 可读可写可执行 | 可读           |
| `MPU_ATTR_RWXRWX` | 可读可写可执行 | 可读可写可执行 |
| `MPU_ATTR_R_X___` | 可读可执行     | 无             |
| `MPU_ATTR_R_XR_X` | 可读可执行     | 可读可执行     |

## MPU模块提供的内存属性
| mem attribute       | 内存属性   |
|---------------------|------------|
| `MPU_ATTR_STRONGLY` | 强序       |
| `MPU_ATTR_DEVICE`   | device属性 |
| `MPU_ATTR_WT`       | 透写       |
| `MPU_ATTR_WB`       | 回写       |

## 内存区间
MPU通过一个一个内存块管理权限，在这里，每个内存块被一个名为`struct mpu_region`的结构体描述，这个结构体定义为：
```
struct mpu_region
    {
    uint32_t addr;
    uint32_t size;
    uint32_t attr;
    };
```
1. 第一项`addr`即此块内存块的起始地址
2. 第二项`size`即此块内存块的大小
3. 第三项`attr`即此块内存块的属性

## MPU模块接口
1. `mpu_task_spawn`用于创建非特权任务，它比`task_create`多一个`regions`参数，这个`regions`是一个`struct mpu_region`的结构体数组，以`{ 0, 0, 0 }`结束。注意由于硬件限制，`size`必须对齐在2的幂上，`addr`对其在`size`上。
2. `mpu_region_add`添加一块内存块给一个任务。

## 使用例子
本例使用多bin。这也是推荐的方案。

注意，本例中使用KEIL作为演示，gnuc，sesa（Segger Embedded Studio for ARM）都也是可以的。

这里以`nrf52dk`板为例子。这块板子的资源如下：

| ROM START  | ROM SIZE   | RAM START  | RAM SIZE   |
|------------|------------|------------|------------|
| 0x00000000 | 0x00100000 | 0x20000000 | 0x00040000 |

我们对这些资源做如下划分：

|            | ROM START  | ROM SIZE   | RAM START  | RAM SIZE   |
|------------|------------|------------|------------|------------|
| kernel     | 0x00000000 | 0x000e0000 | 0x20000000 | 0x00020000 |
| app#1      | 0x000e0000 | 0x00010000 | 0x20020000 | 0x00010000 |
| app#1      | 0x000f0000 | 0x00010000 | 0x20030000 | 0x00010000 |

接下来的步骤演示工程的创建过程

## 创建内核工程
1. `prj -w kernel nrf52dk`创建工程
2. `cd kernel`进入工程目录
3. `prj -s CONFIG_MPU=y CONFIG_SVC=y CONFIG_ROM_SIZE=0x000e0000 CONFIG_RAM_SIZE=0x00020000 CONFIG_KEIL=y`配置工程，也可以使用`prj -m`配置，配置完成后会发现KEIL工程已经生成好了。
4. 修改工程中的样板文件`main.c`添加以下内容：
    ```
    #include <config.h>
    #include <task.h>

    #include <arch/mpu.h>

    task_id mpu_task1;
    task_id mpu_task2;

    int main (void)
        {
        struct mpu_region regions1 [] =
            {
                { 0x000e0000, 0x00010000, MPU_ATTR_R_XR_X },
                { 0x20020000, 0x00010000, MPU_ATTR_RW_RW_ },
                { 0, 0, 0 },
            };

        struct mpu_region regions2 [] =
            {
                { 0x000f0000, 0x00010000, MPU_ATTR_R_XR_X },
                { 0x20030000, 0x00010000, MPU_ATTR_RW_RW_ },
                { 0, 0, 0 },
            };

        mpu_task1 = mpu_task_spawn ("mpu task#1", 20, 0, 0x500,
                                    (int (*) (uintptr_t)) 0xe0001, 0, regions1);
        mpu_task2 = mpu_task_spawn ("mpu task#2", 20, 0, 0x500,
                                    (int (*) (uintptr_t)) 0xf0001, 0, regions2);

        return 0;
        }
    ```
5. 编译工程并配置下载方式后下载
## 创建APP工程
### 创建APP#1
1. `prj -w app1 nrf52dk`创建工程
2. `cd app1`进入工程目录
3. `prj -s CONFIG_USER_APP=y CONFIG_ROM_START=0x000e0000 CONFIG_ROM_SIZE=0x00010000 CONFIG_RAM_START=0x20020000 CONFIG_RAM_SIZE=0x00010000 CONFIG_KEIL=y`配置为user app，并配置rom，ram区间以及配置keil工程，配置完成后keil工程就生成了。打开工程，将编译优化等级改为`-O1`，因为`-O0`编译器会添加很多断点指令，多bin调试不方便。
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

### 创建APP#2
1. `prj -w app2 nrf52dk`创建工程
2. `cd app2`进入工程目录
3. `prj -s CONFIG_USER_APP=y CONFIG_ROM_START=0x000f0000 CONFIG_ROM_SIZE=0x00010000 CONFIG_RAM_START=0x20030000 CONFIG_RAM_SIZE=0x00010000 CONFIG_KEIL=y`配置为user app，并配置rom，ram区间以及配置keil工程，配置完成后keil工程就生成了。打开工程，将编译优化等级改为`-O1`，因为`-O0`编译器会添加很多断点指令，多bin调试不方便。
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

