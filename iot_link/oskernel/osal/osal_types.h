/*
 * osal_types.h
 *
 *  Created on: 2019Äê4ÔÂ26ÈÕ
 *      Author: zhangqf
 */

#ifndef __OSAL_TYPES_H
#define __OSAL_TYPES_H

#ifndef bool_t
#define bool_t int
#endif
#ifndef true
#define true  1
#endif
#ifndef false
#define false 0
#endif
typedef void*  osal_mutex_t;
#define cn_mutex_invalid  NULL

typedef void*  osal_semp_t;
#define cn_semp_invalid   NULL

#define cn_osal_timeout_forever  -1

#endif /* __OSAL_TYPES_H */
