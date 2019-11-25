[![Build Status](https://travis-ci.org/LiteOS/LiteOS_Lab.svg?branch=master)](https://travis-ci.org/LiteOS/LiteOS_Lab)

## SDK简介

Huawei IoT link SDK（下文统一简称SDK）是部署在具备广域网能力、对功耗/存储/计算资源有苛刻限制的终端设备上的轻量级互联互通中间件，您只需调用API接口，便可实现设备快速接入到物联网平台以及数据上报和命令接收等功能。

SDK提供端云协同能力，集成了MQTT、LwM2M、CoAP、mbedtls、LwIP 全套 IoT 互联互通协议栈，且在这些协议栈的基础上，提供了开放 API，用户只需关注自身的应用，而不必关注协议内部实现细节，直接使用SDK封装的API，通过连接、数据上报、命令接收和断开四个步骤就能简单快速地实现与华为OceanConnect云平台的安全可靠连接。使用SDK，用户可以大大减少开发周期，聚焦自己的业务开发，快速构建自己的产品。

## SDK接入云平台开发指南

* [SDK开发指南](./doc/Huawei_IoT_Link_SDK_Developer_Guide.md)

通过SDK中的端云互通组件，可以简单快速地实现与华为 OceanConnect IoT平台安全可靠连接，可以大大减少开发周期，快速构建IoT产品。

* [SDK端云互通组件LwM2M开发指南](./doc/Huawei_IoT_Link_SDK_LwM2M_Developer_Guide_zh.md)
* [SDK端云互通组件MQTT开发指南](./doc/Huawei_IoT_Link_SDK_MQTT_Developer_Guide.md)

## 开源协议

* 遵循BSD-3开源许可协议
* [Huawei LiteOS 知识产权政策](http://developer.huawei.com/ict/cn/site-iot/article/iot-intellectual-property-rights)

## SDK Git入门必读

SDK Commit Message规则和代码贡献流程参考LiteOS。

- [LiteOS Commit Message规则](./doc/LiteOS_Commit_Message.md)

该文档描述如何提交commit到LiteOS仓库，这是LiteOS开发必须遵守的commit规则，否则提交的commit会被驳回。请点链接了解详细信息。

- [Huawei LiteOS代码贡献流程](./doc/LiteOS_Contribute_Guide_GitGUI.md)

该文档描述开发者如何创建自己的仓库，开发然后贡献代码到LiteOS仓库。请点链接了解详细信息。


## 加入我们
* 欢迎提交issue对关心的问题发起讨论，欢迎提交PR参与特性建设
* 如您有合作意向，希望加入Huawei LiteOS生态合作伙伴，请发邮件至liteos@huawei.com，或访问[LiteOS官网](http://www.huawei.com/liteos)，进一步了解详细信息

