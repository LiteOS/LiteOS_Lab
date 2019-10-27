/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#ifndef __SYMN2C_H__
#define __SYMN2C_H__

#define __N_TO_C_0              A
#define __N_TO_C_1              B
#define __N_TO_C_2              C
#define __N_TO_C_3              D
#define __N_TO_C_4              E
#define __N_TO_C_5              F
#define __N_TO_C_6              G
#define __N_TO_C_7              H
#define __N_TO_C_8              I
#define __N_TO_C_9              J
#define __N_TO_C_10             K
#define __N_TO_C_11             L
#define __N_TO_C_12             M
#define __N_TO_C_13             N
#define __N_TO_C_14             O
#define __N_TO_C_15             P
#define __N_TO_C_16             Q
#define __N_TO_C_17             R
#define __N_TO_C_18             S
#define __N_TO_C_19             T
#define __N_TO_C_20             U
#define __N_TO_C_21             V
#define __N_TO_C_22             W
#define __N_TO_C_23             X
#define __N_TO_C_24             Y
#define __N_TO_C_25             Z

/* build up symbols from numbers, for example, SYM_N2C (GPIO, 0) => GPIOA */

#define SYM_N2C(prefix, num)    __CONCAT (prefix, __CONCAT (__N_TO_C_, num))

#define __n_to_c_0              a
#define __n_to_c_1              b
#define __n_to_c_2              c
#define __n_to_c_3              d
#define __n_to_c_4              e
#define __n_to_c_5              f
#define __n_to_c_6              g
#define __n_to_c_7              h
#define __n_to_c_8              i
#define __n_to_c_9              j
#define __n_to_c_10             k
#define __n_to_c_11             l
#define __n_to_c_12             m
#define __n_to_c_13             n
#define __n_to_c_14             o
#define __n_to_c_15             p
#define __n_to_c_16             q
#define __n_to_c_17             r
#define __n_to_c_18             s
#define __n_to_c_19             t
#define __n_to_c_20             u
#define __n_to_c_21             v
#define __n_to_c_22             w
#define __n_to_c_23             x
#define __n_to_c_24             y
#define __n_to_c_25             z

/* build up symbols from numbers, for example, sym_n2c (gpio, 0) => gpioa */

#define sym_n2c(prefix, num)    __CONCAT (prefix, __CONCAT (__n_to_c_, num))

#endif  /* __SYMN2C_H__ */
