## huaweicloud_iot_link SDK开发板移植清单

huaweicloud_iot_link SDK目前已经适配了多款模组多款开发板，多个不同的OS，具体如下所示

## 1 WIFI&蓝牙模组

| 分类              | 硬件   | 上华为云 | 描述                                                         |
| ----------------- | ------ | -------- | ------------------------------------------------------------ |
| 博流  BL602       | 单模组 | MQTT     | [华为云openCPU智联模组_wifi_BL602(RISC-V)_MQTT](https://bbs.huaweicloud.com/blogs/238212) |
|                   | 单模组 | LwM2M    | [华为云openCPU智联模组_wifi_BL602(RISC-V)_LwM2M](https://bbs.huaweicloud.com/blogs/238213) |
|                   | 单模组 | CoAP     | [华为云openCPU智联模组_wifi_BL602(RISC-V)_CoAP](https://bbs.huaweicloud.com/blogs/238214) |
| 上海汉枫 HF-LPX70 | 单模组 | LwM2M    | [华为云openCPU智联模组_wifi_HF-LPX70_RISC-V_LwM2M](https://bbs.huaweicloud.com/blogs/246215) |
|                   | 单模组 | CoAP     | [华为云openCPU智联模组_wifi_HF-LPX70_RISC-V_CoAP](https://bbs.huaweicloud.com/blogs/247263) |
| 瑞昱RTL8720       | 单模组 | MQTT     | [华为云openCPU智联模组_wifi_8720_MQTT](https://bbs.huaweicloud.com/blogs/233458) |
|                   | 单模组 | LwM2M    | [华为云openCPU智联模组_wifi_8720_LwM2M](https://bbs.huaweicloud.com/blogs/236198) |

## 2 NB-IoT&蓝牙模组

| 分类                                 | 硬件   | 上华为云      | 描述                                                         |
| ------------------------------------ | ------ | ------------- | ------------------------------------------------------------ |
| 移远三代NB<br/>BC95-CNV<br/>BC28-CNV | 单模组 | LwM2M         | [华为云all-in-one智联模组_NB-IoT_BC95-CNV(BC28-CNV)_LwM2M_at](https://bbs.huaweicloud.com/blogs/249241) |
|                                      | 单模组 | MQTT          | [华为云all-in-one智联模组_NB-IoT_BC95-CNV(BC28-CNV)_MQTT_at](https://bbs.huaweicloud.com/blogs/249333) |
|                                      | 单模组 | openCPU+LwM2M | [华为云all-in-one智联模组_NB-IoT_BLE_BC95-CNV_BC28-CNV_openCPU](https://bbs.huaweicloud.com/blogs/253691) |
|                                      | 单模组 | 蓝牙APP+LwM2M | [华为云all-in-one智联模组_NB-IoT_BLE_BC95-CNV_BC28-CNV_蓝牙APP](https://bbs.huaweicloud.com/blogs/253694) |

## 3 MCU

| 分类             | 硬件     | 上华为云 | 描述                                                         |
| ---------------- | -------- | -------- | ------------------------------------------------------------ |
| 航顺  HK32F103Vx | MCU+WIFI | LwM2M    | [华为云AIoT智联开发板_mcu_HK32F103Vx](https://bbs.huaweicloud.com/blogs/244682) |

## 4 PLC 无线网关

| 分类                               | 硬件      | 上华为云        | 描述                                                         |
| ---------------------------------- | --------- | --------------- | ------------------------------------------------------------ |
| 欧智通 <br />3121N-IED     3121N-H | PLC+WIFI  | PLC+WIFI        | [华为云smart智联PLC无线网关_plc_3121N-H(3121N-IED)](https://bbs.huaweicloud.com/blogs/245850) |
|                                    | PLC+NB    | PLC+NB-IoT+蓝牙 | [华为云smart智联PLC无线网关_plc_3121N-H(3121N-IED)](https://bbs.huaweicloud.com/blogs/254821) |
|                                    | PLC+Cat.1 | PLC+Cat.1       | [华为云smart智联Cat.1+PLC无线网关_3121N-IED_MC615-CN-L610-CN（面向Cat.1+PLC无线网关）](https://bbs.huaweicloud.com/blogs/296169) |

## 5 语音识别

| 分类                | 硬件            | 上华为云    | 描述                                                         |
| ------------------- | --------------- | ----------- | ------------------------------------------------------------ |
| 声达创新<br/>AI语音 | AI语音+WIFI+LCD | AI语音+MQTT | [（视频演示）语音识别、RISC-V架构WiFi芯片、LCD显示屏融合案例](https://bbs.huaweicloud.com/blogs/259858) |
|                     |                 |             | [华为云smart语音识别_AI_Voice_wifi_RISC-V_LCD_ST7789](https://bbs.huaweicloud.com/blogs/261451) |

## 6 Cat.1模组

| 分类   | 硬件   | 上华为云 | 描述                                                         |
| ------ | ------ | -------- | ------------------------------------------------------------ |
| 广和通 | 单模组 | 基础操作 | [华为云openCPU智联模组_Cat.1_MC615-CN（面向基础操作）](https://bbs.huaweicloud.com/blogs/263473) |
|        | 单模组 | MQTT     | [华为云openCPU智联模组_Cat.1_MC615-CN-L610-CN（面向上华为云）](https://bbs.huaweicloud.com/blogs/267413) |

## 7 LittlevGL

| 分类        | 硬件               | 上华为云 | 描述                                                         |
| ----------- | ------------------ | -------- | ------------------------------------------------------------ |
| 博流  BL602 | LittlevGL+WIFI+LCD | MQTT     | [（视频演示）云弹幕、LittlevGL、RISC-V架构WiFi芯片、LCD显示屏](https://bbs.huaweicloud.com/blogs/262841) |

## 8 OTA

| 分类   | 硬件   | 上华为云 | 描述                                                         |
| ------ | ------ | -------- | ------------------------------------------------------------ |
| 广和通 | 单模组 | MQTT     | [华为云openCPU智联模组_Cat.1_MC615-CN_L610-CN_OTA升级（面向OTA在线升级）](https://bbs.huaweicloud.com/blogs/293310) |

## 9 GPS定位

| 分类   | 硬件   | 上华为云 | 描述                                                         |
| ------ | ------ | -------- | ------------------------------------------------------------ |
| 广和通 | 单模组 | MQTT     | [华为云openCPU智联模组_Cat.1_MC615-CN-L610-CN_GPS定位器](https://bbs.huaweicloud.com/blogs/297178) |

## 10 TTS语音播报

| 分类   | 硬件   | 上华为云 | 描述                                                         |
| ------ | ------ | -------- | ------------------------------------------------------------ |
| 广和通 | 单模组 | MQTT     | [华为云openCPU智联模组_Cat.1_MC615-CN-L610-CN_云喇叭TTS语音](https://bbs.huaweicloud.com/blogs/296429) |

## 11 环境温湿度

| 分类        | 硬件   | 上华为云 | 描述                                                         |
| ----------- | ------ | -------- | ------------------------------------------------------------ |
| 博流  BL602 | 单模组 | MQTT     | [华为云openCPU智联模组_wifi_BL602(RISC-V)_MQTT](https://bbs.huaweicloud.com/blogs/238212) |

## 12 OpenHarmony

| 分类           | 硬件   | 上华为云 | 描述                                                         |
| -------------- | ------ | -------- | ------------------------------------------------------------ |
| BearPi-HM_Nano | 单模组 | MQTT     | [OpenHarmony 3.0对接华为云](https://bbs.huaweicloud.com/blogs/308673) |



