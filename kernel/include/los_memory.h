/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2017>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
#ifndef _LOS_MEMORY_H
#define _LOS_MEMORY_H
#include "los_base.h"
#ifdef CONFIG_SLAB
#include <los_slab.ph>
#endif
#include <los_heap.ph>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef LOSCFG_NEW_HEAP
/**
 *@ingroup los_memory
 *@brief calculate heap information.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to calculate heap information.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>One parameter of this interface is a pointer, it should be a correct value, otherwise, the system may be abnormal.</li>
 *</ul>
 *
 *@param  status        [OUT] Type  #LOS_MEM_STATUS* Pointer to the heap status structure to be obtained.
 *@param  pPool    [IN] Pointer to the memory pool that contains the memory block to be allocated.
 *
 *@retval #LOS_OK        The heap status calculate success.
 *@retval #LOS_NOK       The heap status calculate with some error.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemAlloc | LOS_MemRealloc | LOS_MemFree
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_MemStatisticsGet(VOID *pPool, LOS_MEM_STATUS *pstStatus);


/**
 *@ingroup los_memory
 *@brief calculate heap max free block size.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to calculate heap max free block size.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param  None.
 *
 *@retval #UINT32        The  max free block size.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemAlloc | LOS_MemRealloc | LOS_MemFree
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_MemGetMaxFreeBlkSize(VOID *pPool);
#endif

/**
 *@ingroup los_memory
 *@brief Initialize dynamic memory.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to initialize the dynamic memory of a doubly linked list.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The uwSize parameter value should match the following two conditions : 1) Be less than or equal to the Memory pool size; 2) Be greater than the size of OS_MEM_MIN_POOL_SIZE.</li>
 *<li>Call this API when dynamic memory needs to be initialized during the startup of Huawei LiteOS.</li>
 *<li>The parameter input must be four byte-aligned.</li>
 *<li>The init area [pPool, pPool + uwSize] should not conflict with other pools.</li>
 *</ul>
 *
 *@param pPool          [IN] Starting address of memory.
 *@param uwSize         [IN] Memory size.
 *
 *@retval #LOS_NOK    The dynamic memory fails to be initialized.
 *@retval #LOS_OK     The dynamic memory is successfully initialized.
 *@par Dependency:
 *<ul>
 *<li>los_memory.h: the header file that contains the API declaration.</li>
 *</ul>
 *@see None.
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_MemInit(VOID *pPool, UINT32 uwSize);

/**
 *@ingroup los_memory
 *@brief Allocate dynamic memory.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to allocate a memory block of which the size is specified.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_MemInit.</li>
 *<li>The size of the input parameter uwSize can not be greater than the memory pool size that specified at the second input parameter of LOS_MemInit.</li>
 *<li>The size of the input parameter uwSize must be four byte-aligned.</li>
 *</ul>
 *
 *@param  pPool    [IN] Pointer to the memory pool that contains the memory block to be allocated.
 *@param  uwSize   [IN] Size of the memory block to be allocated (unit: byte).
 *
 *@retval #NULL          The memory fails to be allocated.
 *@retval #VOID*         The memory is successfully allocated with the starting address of the allocated memory block returned.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemRealloc | LOS_MemAllocAlign | LOS_MemFree
 *@since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_MemAlloc (VOID *pPool, UINT32  size);

/**
 *@ingroup los_memory
 *@brief Free dynamic memory.
 *
 *@par Description:
 *<li>This API is used to free specified dynamic memory that has been allocated.</li>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_MemInit.</li>
 *<li>The input pMem parameter must be allocated by LOS_MemAlloc or LOS_MemAllocAlign or LOS_MemRealloc.</li>
 *</ul>
 *
 *@param  pPool  [IN] Pointer to the memory pool that contains the dynamic memory block to be freed.
 *@param  pMem   [IN] Starting address of the memory block to be freed.
 *
 *@retval #LOS_NOK          The memory block fails to be freed
 *@retval #LOS_OK           The memory block is successfully freed.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemAlloc | LOS_MemRealloc | LOS_MemAllocAlign
 *@since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_MemFree(VOID *pPool, VOID *pMem);

/**
 *@ingroup los_memory
 *@brief Re-allocate a memory block.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to allocate a new memory block of which the size is specified by size if the original memory block size is insufficient. The new memory block will copy the data in the original memory block of which the address is specified by ptr. The size of the new memory block determines the maximum size of data to be copied. After the new memory block is created, the original one is freed.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_MemInit.</li>
 *<li>The input pPtr parameter must be allocated by LOS_MemAlloc.</li>
 *<li>The size of the input parameter uwSize can not be greater than the memory pool size that specified at the second input parameter of LOS_MemInit.</li>
 *<li>The size of the input parameter uwSize must be aligned as follows: 1) if the pPtr is allocated by LOS_MemAlloc, it must be four byte-aligned; 2) if the pPtr is allocated by LOS_MemAllocAlign, it must be aligned with the size of the input parameter uwBoundary of LOS_MemAllocAlign.</li>
 *</ul>
 *
 *@param  pPool      [IN] Pointer to the memory pool that contains the original and new memory blocks.
 *@param  pPtr       [IN] Address of the original memory block.
 *@param  uwSize     [IN] Size of the new memory block.
 *
 *@retval #NULL          The memory fails to be re-allocated.
 *@retval #VOID*         The memory is successfully re-allocated with the starting address of the new memory block returned.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemAlloc | LOS_MemAllocAlign | LOS_MemFree
 *@since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_MemRealloc(VOID *pPool, VOID *pPtr, UINT32 uwSize);

/**
 *@ingroup los_memory
 *@brief Allocate aligned memory.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to allocate memory blocks of specified size and of which the starting addresses are aligned on a specified boundary.</li>
 *</ul>
 *@attention
 *<ul>
 *<li>The input pPool parameter must be initialized via func LOS_MemInit.</li>
 *<li>The size of the input parameter uwSize can not be greater than the memory pool size that specified at the second input parameter of LOS_MemInit.</li>
 *<li>The alignment parameter value must be a power of 2 with the minimum value being 4.</li>
 *</ul>
 *
 *@param  pPool       [IN] Pointer to the memory pool that contains the memory blocks to be allocated.
 *@param  uwSize      [IN] Size of the memory to be allocated.
 *@param  uwBoundary  [IN] Boundary on which the memory is aligned.
 *
 *@retval #NULL          The memory fails to be allocated.
 *@retval #VOID*         The memory is successfully allocated with the starting address of the allocated memory returned.
 *@par Dependency:
 *<ul><li>los_memory.h: the header file that contains the API declaration.</li></ul>
 *@see LOS_MemAlloc | LOS_MemRealloc | LOS_MemFree
 *@since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_MemAllocAlign (VOID *pPool, UINT32 uwSize, UINT32 uwBoundary);

#ifdef LOSCFG_NEW_HEAP

/**
 *@ingroup los_memory
 *@brief Print infomation about a pool.
 *
 *@par Description:
 *<ul>
 *<li>This API is used to print infomation about a pool.</li>
 *</ul>
 *
 *@param pPool          [IN] Starting address of pool, if NULL, use kernel pool.
 *@param bShowDetail    [IN] If show detailed information.
 *<ul>
 *<li>los_memory.h: the header file that contains the API declaration.</li>
 *</ul>
 *@see None.
 */

extern VOID LOS_MemInfo (VOID * pPool, BOOL bShowDetail);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_MEMORY_H */
