/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * OpenArkCompiler is licensed under the Mulan PSL v1. 
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

#include "secureprintoutput.h"

/*
 * <FUNCTION DESCRIPTION>
 *    The vsprintf_s function is equivalent to the vsprintf function
 *    except for the parameter destMax and the explicit runtime-constraints violation
 *    The vsprintf_s function takes a pointer to an argument list, and then formats
 *    and writes the given data to the memory pointed to by strDest.
 *    The function differ from the non-secure versions only in that the secure
 *    versions support positional parameters.
 *
 * <INPUT PARAMETERS>
 *    strDest                Storage location for the output.
 *    destMax                Size of strDest
 *    format                 Format specification.
 *    argList                   pointer to list of arguments
 *
 * <OUTPUT PARAMETERS>
 *    strDest                is updated
 *
 * <RETURN VALUE>
 *    return  the number of characters written, not including the terminating null character,
 *    return -1  if an  error occurs.
 *
 * If there is a runtime-constraint violation, strDest[0] will be set to the '\0' when strDest and destMax valid
 */
int vsprintf_s(char *strDest, size_t destMax, const char *format, va_list argList)
{
    int retVal;               /* If initialization causes  e838 */

    if (format == NULL || strDest == NULL || destMax == 0 || destMax > SECUREC_STRING_MAX_LEN) {
        if (strDest != NULL && destMax > 0 && destMax <= SECUREC_STRING_MAX_LEN) {
            strDest[0] = '\0';
        }
        SECUREC_ERROR_INVALID_PARAMTER("vsprintf_s");
        return -1;
    }

    retVal = SecVsnprintfImpl(strDest, destMax, format, argList);

    if (retVal < 0) {
        strDest[0] = '\0';
        if (retVal == SECUREC_PRINTF_TRUNCATE) {
            /* Buffer is too small */
            SECUREC_ERROR_INVALID_RANGE("vsprintf_s");
        }
        SECUREC_ERROR_INVALID_PARAMTER("vsprintf_s");
        return -1;
    }

    return retVal;
}
#if SECUREC_IN_KERNEL
EXPORT_SYMBOL(vsprintf_s);
#endif


