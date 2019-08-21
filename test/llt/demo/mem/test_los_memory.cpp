/* Includes -----------------------------------------------------------------*/
#include <stdio.h>

#include "gtest/gtest.h"

#include "los_typedef.h"
#include "los_memory.h"
#include "los_adapter.h"

#include "mem.h"

/* Defines ------------------------------------------------------------------*/
#ifdef OS_SYS_MEM_SIZE
#undef OS_SYS_MEM_SIZE
#endif
#define OS_SYS_MEM_SIZE     0x0024000          // size 200k

/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
UINT8   g_sysMem1[OS_SYS_MEM_SIZE];
UINT8 *m_aucSysMem1 = g_sysMem1;

UINT8   g_sysPhysMem[OS_SYS_MEM_SIZE];
/* Extern variables ---------------------------------------------------------*/
extern "C"
{
    UINT32 LOS_MemStatisticsGet(VOID *pPool, LOS_MEM_STATUS *pstStatus);
    UINT32 LOS_MemGetMaxFreeBlkSize(VOID *pPool);
#if (LOSCFG_HEAP_MEMORY_PEAK_STATISTICS == YES)
    UINT32 osHeapGetHeapMemoryPeak(VOID *pPool);
#endif
}
/* Global variables ---------------------------------------------------------*/
const struct phys_mem system_phys_mem [] = {
    {(unsigned long)g_sysPhysMem, (unsigned long)(g_sysPhysMem + OS_SYS_MEM_SIZE - 1)}
};

/* Private function prototypes ----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
class TestLosMemory : public ::testing::Test
{
protected:
    void SetUp() {
		LOS_MemInit(m_aucSysMem0, OS_SYS_MEM_SIZE);
	};

	void TearDown() {
#if (LOSCFG_MEM_MUL_POOL == YES)
		LOS_MemDeInit(m_aucSysMem0);
#endif
	};
};

#if (LOSCFG_MEM_MUL_POOL == YES)
TEST_F(TestLosMemory, LOS_MemInit)
{
    UINT32 uwRet;
    uwRet = LOS_MemInit(m_aucSysMem1, OS_SYS_MEM_SIZE);
    EXPECT_EQ(LOS_OK, uwRet);
    uwRet = LOS_MemPoolList();
    EXPECT_EQ(2, uwRet);

    uwRet = LOS_MemDeInit(NULL);
    EXPECT_EQ(LOS_NOK, uwRet);

    uwRet = LOS_MemInit(m_aucSysMem1, 1);
    EXPECT_EQ(LOS_NOK, uwRet);
    uwRet = LOS_MemInit(m_aucSysMem1 + 1, OS_SYS_MEM_SIZE);
    EXPECT_EQ(LOS_NOK, uwRet);
    uwRet = LOS_MemInit(m_aucSysMem1 + 4, OS_SYS_MEM_SIZE);
    EXPECT_EQ(LOS_NOK, uwRet);

    uwRet = LOS_MemDeInit(m_aucSysMem1);
    EXPECT_EQ(LOS_OK, uwRet);
}
#endif

TEST_F(TestLosMemory, LOS_MemAlloc)
{
    UINT32 uwRet;
    UINT8 *pData = NULL;

    pData = (UINT8 *)LOS_MemAlloc(NULL, 10);
    ASSERT_TRUE(pData == NULL);
    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 10);
    ASSERT_TRUE(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    ASSERT_EQ(LOS_OK, uwRet);

    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 256);
    ASSERT_TRUE(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    ASSERT_EQ(LOS_OK, uwRet);
}

TEST_F(TestLosMemory, osMemSystemInit)
{
    UINT32 uwRet;
    uwRet  = osMemSystemInit();
    ASSERT_TRUE(uwRet == LOS_NOK);
}

TEST_F(TestLosMemory, LOS_MemAllocAlign)
{
    UINT32 uwRet;
    UINT8 *pData = NULL;

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 1);
    ASSERT_TRUE(pData == NULL);
    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 4);
    ASSERT_TRUE(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    ASSERT_EQ(LOS_OK, uwRet);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 8);
    ASSERT_TRUE(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    ASSERT_EQ(LOS_OK, uwRet);
}

TEST_F(TestLosMemory, LOS_MemRealloc)
{
    UINT32 uwRet;
    UINT8 *pData = NULL;
    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 10);
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 8); // cutdown
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 10); // expand
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 0); // free
    ASSERT_TRUE(pData == NULL);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 8);
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 168); // cutdown
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 0); // free
    ASSERT_TRUE(pData == NULL);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 256, 8);
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 168); // cutdown
    ASSERT_TRUE(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 0); // free
    ASSERT_TRUE(pData == NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, NULL, 20);
    ASSERT_TRUE(pData != NULL);
}

TEST_F(TestLosMemory, LOS_MemStatisticsGet)
{
    UINT32 uwRet;
    LOS_MEM_STATUS status;
    UINT8 *pData = NULL;

    uwRet = LOS_MemStatisticsGet(m_aucSysMem0, &status);
    ASSERT_EQ(LOS_OK, uwRet);
    printf("---+++--- memory (%p) statistics ---+++---\n", m_aucSysMem0);
    printf("totalSize   : 0x%x\n", status.totalSize);
    printf("usedSize    : 0x%x\n", status.usedSize);
    printf("freeSize    : 0x%x\n", status.freeSize);
    printf("allocCount  : %d\n", status.allocCount);
    printf("freeCount   : %d\n", status.freeCount);
    uwRet = LOS_MemGetMaxFreeBlkSize(m_aucSysMem0);
    printf("memory max free block size : %d\n", uwRet);

    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 1024);
    ASSERT_TRUE(pData != NULL);
    uwRet = LOS_MemGetMaxFreeBlkSize(m_aucSysMem0);
    printf("memory max free block size : %d\n", uwRet);
#if (LOSCFG_HEAP_MEMORY_PEAK_STATISTICS == YES)
    uwRet = osHeapGetHeapMemoryPeak(m_aucSysMem0);
    printf("memory max heap used : %d\n", uwRet);
#endif
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    ASSERT_EQ(LOS_OK, uwRet);
}

/* Private functions --------------------------------------------------------*/

