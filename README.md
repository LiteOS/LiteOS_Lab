[![Build Status](https://travis-ci.org/LiteOS/LiteOS_Lab.svg?branch=master)](https://travis-ci.org/LiteOS/LiteOS_Lab)

## SDK简介

IoT Device SDK Tiny（下文统一简称SDK）是部署在具备广域网能力、对功耗/存储/计算资源有苛刻限制的终端设备上的轻量级互联互通中间件，您只需调用API接口，便可实现设备快速接入到物联网平台以及数据上报和命令接收等功能。

SDK提供端云协同能力，集成了MQTT、LwM2M、CoAP、mbedtls、LwIP 全套 IoT 互联互通协议栈，且在这些协议栈的基础上，提供了开放 API，用户只需关注自身的应用，而不必关注协议内部实现细节，直接使用SDK封装的API，通过连接、数据上报、命令接收和断开四个步骤就能简单快速地实现与华为云物联网平台的安全可靠连接。使用SDK，用户可以大大减少开发周期，聚焦自己的业务开发，快速构建自己的产品。

## SDK接入云平台开发指南

* [SDK开发指南](./docs/IoT_Device_SDK_Tiny_Developer_Guide.md)
* [极简版本SDK移植指南](./docs/Light_SDK_Developer_Guide.md)
* [SDK移植案例](./docs/SDK_Demos_List.md)
* [上电即上华为云IoT开发板列表](https://bbs.huaweicloud.com/blogs/301160)

## 开源协议

* 遵循BSD-3开源许可协议
