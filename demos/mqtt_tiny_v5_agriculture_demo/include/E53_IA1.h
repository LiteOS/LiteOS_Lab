/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __E53_IA1_H__
#define __E53_IA1_H__

/* 寄存器宏定义 -------------------------------------------------------------------- */
#define SHT30_ADDR 0x44
#define BH1750_ADDR 0x23

typedef enum {
    OFF = 0,
    ON
} E53IA1Status;

/* E53_IA1传感器数据类型定义 ------------------------------------------------------------ */
typedef struct {
    float Lux;         // 光照强度
    float Humidity;    // 湿度
    float Temperature; // 温度
} E53IA1Data;

int E53IA1Init(void);
int E53IA1ReadData(E53IA1Data *ReadData);
void LightStatusSet(E53IA1Status status);
void MotorStatusSet(E53IA1Status status);


#endif /* __E53_IA1_H__ */
