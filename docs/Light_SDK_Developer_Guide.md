## 轻量级SDK移植指南

IoT Device SDK Tiny具备可裁剪特性，可以根据需要进行定制化组件，减少内存占用，本文将介绍轻量级SDK移植指南。

## SDK目录简介

首先看一下SDK的源码目录，具体如下表所示。

| 一级目录   | 二级目录          | 三级目录 | 说明                                                         |
| :--------- | :---------------- | :------- | ------------------------------------------------------------ |
| LiteOS_Lab | at                | ——       | AT指令框架实现                                               |
|            | cJSON             | ——       | cJSON                                                        |
|            | crc               | ——       | crc校验                                                      |
|            | demos             | ——       | 示例                                                         |
|            | driver            | ——       | 驱动框架                                                     |
|            | docs              | ——       | 存放使用文档及API说明等文档                                  |
|            | fs                | ——       | 文件系统，含VFS、SPIffs、RAMfs、KIfs、DEVfs、FATfs           |
|            | inc               | ——       | 存放内核内部使用头文件                                       |
|            | link_log          | ——       | 日志                                                         |
|            | link_misc         | ——       | 杂项                                                         |
|            | link_ota          | ——       | OTA升级代码实现                                              |
|            | network           | coap     | CoAP的适配及协议实现                                         |
|            |                   | tls      | mbedtls的适配及协议实现                                      |
|            |                   | lwm2m    | LwM2M的开源协议栈wakaama的适配                               |
|            |                   | mqtt     | MQTT的适配及协议实现                                         |
|            |                   | tcpip    | TCPIP适配及协议栈实现、lwIP驱动、OS适配及协议栈实现、MacOS_socket适配及协议栈实现 |
|            | oc                | oc_coap  | CoAP协议适配华为云物联网平台                                 |
|            |                   | oc_lwm2m | LwM2M协议适配华为华为云物联网平台                            |
|            |                   | oc_mqtt  | MQTT协议适配华为华为云物联网平台                             |
|            | os                | osal     | IoT Device SDK Tiny的OS适配                                  |
|            |                   | xxos     | 包括freertos、linux、liteos、macos、novaos、ucos_ii等操作系统的适配 |
|            | queue             | ——       | queue组件的代码实现                                          |
|            | secure_c          | ——       | C安全函数库                                                  |
|            | shell             | ——       | shell组件代码实现                                            |
|            | stimer            | ——       | stimer组件代码实现                                           |
|            | storage           | ——       | 存储分区                                                     |
|            | usip              | ——       | usip协议                                                     |
|            | iot.mk            | ——       | Makefile                                                     |
|            | iot_config.h      | ——       | 宏定义                                                       |
|            | iot_link_config.h | ——       |                                                              |
|            | link_main.c       | ——       | SDK初始化入口                                                |
|            | README.md         | ——       | SDK 简介                                                     |

根据源码目录，进行逐层分析轻量级SDK的必要源码移植，使用该SDK的核心功能在于对接华为云物联网平台，而目前各大物联网平台常用的协议为MQTT协议，本文将以MQTT协议为例分析轻量级SDK的移植。具体需要如下文件。

- 则此时网络层和OC层必定要使用MQTT。
- 本SDK脱离OS，因此必定需要osal层以及相应的操作系统适配文件，如该SDK中的 xx_imp.c文件。
- MQTT底层采用tcpip因此也需要移植或者适配TCPIP
- 同时MQTT协议的数据收发都是使用JSON格式完成的，因此需要cJson。
- 如果加密还需要使用tls文件,在生成username和password的时候需要使用hash256算法，此时需要调用mbedtls
- 对接云的oc层各个协议中均使用到了队列，因此要用到queue。
- inc中存放的是内核内部使用头文件，link_misc以及日志link_log也是较为必要的。
- 宏定义开关可以参考makefile文件以及iot_config.h文件。
- 至于其他组件如at、stimer、shell等可以根据用户需求自行添加

因此轻量级SDK中应该包含如下文件夹：cJSON、inc、link_log、link_misc、network中的tls、mqtt、tcpip等文件、oc中的oc_mqtt以及os中的osal文件、queue文件。

## 移植步骤

### SDK编译简介

原始的SDK采用控制编译文件的工具为GNU Make，即makefile，对于该SDK主makefile文件即为iot.mk，具体如下。

```makefile
iot_link_root = $(SDK_DIR)/iot_link

#configure the common includes
include $(iot_link_root)/inc/inc.mk

#configure the misc
include $(iot_link_root)/link_misc/link_misc.mk

#configure the log
include $(iot_link_root)/link_log/link_log.mk

#configure the operation system for the iot_link
include $(iot_link_root)/os/os.mk

#configure the shell for the iot_link
include $(iot_link_root)/shell/shell.mk

#configure the soft timer for the iot_link
include $(iot_link_root)/stimer/stimer.mk

#configure the queue
include $(iot_link_root)/queue/queue.mk

#configure the driver famework for the iot_link
include $(iot_link_root)/driver/driver.mk

#configure the AT famework for the iot_link
include $(iot_link_root)/at/at.mk

#configure the cJSON 
include $(iot_link_root)/cJSON/cJSON.mk 

#configure th crc
include $(iot_link_root)/crc/crc.mk 

#configure the network
include $(iot_link_root)/network/network.mk

#configure the oceanconnect
include $(iot_link_root)/oc/oc.mk

#configure the ota	
include $(iot_link_root)/link_ota/build.mk

#configure demos
include $(iot_link_root)/demos/demos.mk

#configure the secure clib
include $(iot_link_root)/secure_c/build.mk
iot_link_src  = ${wildcard $(iot_link_root)/*.c} 
C_SOURCES += $(iot_link_src)	
		
iot_link_inc = -I $(iot_link_root)
C_INCLUDES += $(iot_link_inc)
```

可以看到上述文件每个include导入的又是一个makefile文件，并且添加了注释，可见SDK编译流程采用的也是层层深入这种方式，打开里面具体的makefile文件可以看到其中包含必要的源文件、头文件、编译参数以及宏定义开关等。

#### os 适配

IoT Device SDK Tiny本身是不局限于某一款具体的物联网操作系统的，但是其运行是依赖于操作系统的，因此需要进行OS的相关适配。当然用户可以根据模组、MCU厂商的需求使用其内置OS，适配IoT Device SDK Tiny实现对接华为云等进行一系列的云服务的使用。目前IoT Device SDK Tiny已经适配了多款OS,包括LiteOS、FreeRTOS、MacOS、Linux、NovaOS、ucos_ii等。

IoT Device SDK Tiny提供了osal（操作系统抽象层），实现了操作系统与SDK组件的隔离。因此厂商可以直接将想要使用的操作系统注册到osal中，即可实现对SDK组件的使用。其中包括与操作系统内核强相关的任务操作、信号量、互斥锁、内存管理接口等内容。在适配过程中可以参考已经适配好的操作系统imp文件。由于本SDK的队列queue采用互斥锁和信号量加以实现，因此无需对队列进行适配。最终调用顺序如下 osal_init() ---> os_imp_init() ---> osal_install()，其中os_imp_init是弱符号函数，需要用户根据自身需要进行实现，在其中调用osal_install()函数实现系统服务的注册。

#### TCP/IP 适配

对于IoT Device SDK Tiny而言，其网络协议栈决定于底层操作系统，在使用三方操作系统时，可以将相关的网络传输接口进行抽象，提供相关的协议栈接口即可。IoT Device SDK Tiny中调用的所有网络传输的接口，通过注册机制，最终都会调用到用户注册的函数。

IoT Device SDK Tiny也提供了抽象层sal，用户可以调用link_sal_install函数进行TCP/IP功能注册，同时用户必须实现link_tcpip_imp_init函数，该函数是一个弱符号函数。在该SDK初始化的过程中，调用顺序为：link_tcp_ip_init ---> link_tcpip_imp_init ---> link_sal_install，在初始化完毕之后，才可以使用TCP/IP相关的功能。详细参考sal_imp.h头文件，同时我们已经适配了LWIP、ESP8266\L716等软件或者模组提供的TCP/IP功能，如果您有第三方的组件或者模组，可以参考实现。

#### 适配中的细节问题

1 不同的操作系统对于任务优先级的规定是不同的，因此IoT Device SDK Tiny中用到的创建任务的函数调用中赋予的优先级要进行相应的改变，目前本SDK默认优先级顺序为优先级数在0-31，0位最高优先级，31为最低优先级。

2 有些编译架构对于虚引用函数的支持以及编译方式可能略有不同，需要根据需求进行自行修改。

3 对于上述组件，如果本身的SDK中已经具备相应的组件可以根据需求进行适配、裁剪等操作。

4 编译架构的不同，可以保留源文件以及库文件，根据自己的需求进行移植适配以及使能，下面仅仅给个iot_config.h文件的参考示例，示例中以对接接口以V5版本接口为例。这里可以重点关注一下SDK的入口函数link_main.c，其中包含了如何通过宏定义控制哪些文件参与编译。

```c
//enable cJSON
#define CONFIG_CJSON_ENABLE 1

//enable LinkLog 
#define CONFIG_LINKLOG_ENABLE 1

//enable queue
#define CONFIG_LINKQUEUE_ENABLE 1

//enable TLS_AL link_main
#define CONFIG_TLS_AL_ENABLE 1
#define CONFIG_MBEDTLS_ENABLE 1
#define CONFIG_MBEDTLS_PSK 1


// enable MQTT link_main
#define CONFIG_PAHO_MQTT 1
#define CONFIG_MQTT_AL_ENABLE 1
 
// enable tcpip
#define CONFIG_TCPIP_AL_ENABLE 1

//choose paho_mqtt or lite_mqtt
//#define CONFIG_PAHO_MQTT 1  


//paho_mqtt_port.c use them
#define CONFIG_PAHO_CONNECT_TIMEOUT 10000
#define CONFIG_PAHO_CMD_TIMEOUT 10000
#define CONFIG_PAHO_LOOPTIMEOUT 10
#define CONFIG_PAHO_SNDBUF_SIZE 2048
#define CONFIG_PAHO_RCVBUF_SIZE 2048

//enable OC_MQTT link_main 
#define CONFIG_OCMQTT_ENABLE 1

//mqtt the profile function tools and implement component oc_mqtt.mk
#define CONFIG_OC_TINYMQTTV5_ENABLE 1 
#define CONFIG_OC_MQTTV5_PROFILE 1

#define CONFIG_IOT_LINK_CONFIGFILE "iot_config.h"
```
