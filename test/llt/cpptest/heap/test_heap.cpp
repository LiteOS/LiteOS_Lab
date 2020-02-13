/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-7    maminjie    The first version
 */

#include "test_heap.h"

#include "los_memory.h"

// #include "heap.h"    // chunk.h->list.h->__dlist_add_at :
                        // the param new is conflicted with the c++ keyword new
                        // so, copy the defines of struct mem_stat

extern "C"
{
    typedef struct heap heap_t;

#if (LOSCFG_MEM_STATISTICS == YES)
    typedef struct mem_stat
    {
        unsigned int       free_chunks;
        size_t             free_size;
        unsigned int       busy_chunks;
        size_t             busy_size;
        size_t             max_busy_size;
        size_t             max_free_size;
        unsigned long long cum_allocated;
        unsigned long long cum_size_allocated;
        unsigned long long cum_freed;
        unsigned long long cum_size_freed;
    } mem_stat_t;

    extern int    heap_stat_get    (heap_t * heap, mem_stat_t * stat);
#endif //LOSCFG_MEM_STATISTICS == YES

    extern int heap_init (heap_t * heap);
    extern int heap_add (heap_t * heap, char * buff, size_t size);
    extern int heap_free (heap_t * heap, char * mem);
}

/* defines */

#define TEST_MEM_SIZE     0x0024000          // size 200k

/* variable */

UINT8  g_sysMem1[TEST_MEM_SIZE] = {0};
UINT8 *m_aucSysMem1 = g_sysMem1;

/* functions */

TestLosHeap::TestLosHeap()
{
    osMemSystemInit(); // init first
    LOS_MemInit(m_aucSysMem0, OS_SYS_MEM_SIZE);

    TEST_ADD(TestLosHeap::test_LOS_MemInit);
    TEST_ADD(TestLosHeap::test_LOS_MemAlloc);
    TEST_ADD(TestLosHeap::test_LOS_MemAllocAlign);
    TEST_ADD(TestLosHeap::test_LOS_MemRealloc);
    TEST_ADD(TestLosHeap::test_LOS_MemFree);
    TEST_ADD(TestLosHeap::test_LOS_MemStatisticsGet);
    TEST_ADD(TestLosHeap::test_heap);
}

TestLosHeap::~TestLosHeap()
{
    // LOS_MemDeInit(m_aucSysMem0);
}

void TestLosHeap::setup()
{

}

void TestLosHeap::tear_down()
{

}

void TestLosHeap::test_LOS_MemInit(void)
{
    TEST_ASSERT_EQUALS(LOS_OK, LOS_MemInit(m_aucSysMem1, TEST_MEM_SIZE));
    // TEST_ASSERT_EQUALS(2, LOS_MemPoolList());                // not implemented
    // TEST_ASSERT_EQUALS(LOS_NOK, LOS_MemDeInit(NULL));        // not implemented
    TEST_ASSERT_EQUALS(LOS_NOK, LOS_MemInit(m_aucSysMem1, 1));
    // TEST_ASSERT_EQUALS(LOS_OK, LOS_MemDeInit(m_aucSysMem1));
}

void TestLosHeap::test_LOS_MemAlloc(void)
{
    UINT32 uwRet;
    UINT8 *pData = NULL;
    pData = (UINT8 *)LOS_MemAlloc(NULL, 10);
    TEST_ASSERT(pData == NULL);
    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 0);
    TEST_ASSERT(pData == NULL);
    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, OS_SYS_MEM_SIZE);
    TEST_ASSERT(pData == NULL);
    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 10);
    TEST_ASSERT(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    TEST_ASSERT(uwRet == LOS_OK);
    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 256);
    TEST_ASSERT(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    TEST_ASSERT(uwRet == LOS_OK);
}

void TestLosHeap::test_LOS_MemAllocAlign(void)
{
    UINT32 uwRet;
    UINT8 *pData = NULL;

    pData = (UINT8 *)LOS_MemAllocAlign(NULL, 10, 1);
    TEST_ASSERT(pData == NULL);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 1);
    TEST_ASSERT(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    TEST_ASSERT(uwRet == LOS_OK);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 13);
    TEST_ASSERT(pData == NULL);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 4);
    TEST_ASSERT(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    TEST_ASSERT(uwRet == LOS_OK);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 8);
    TEST_ASSERT(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    TEST_ASSERT(uwRet == LOS_OK);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, OS_SYS_MEM_SIZE >> 1, 8);
    TEST_ASSERT(pData != NULL);
    uwRet = LOS_MemFree(m_aucSysMem0, pData);
    TEST_ASSERT(uwRet == LOS_OK);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 12);
    TEST_ASSERT(pData == NULL);


    UINT8 *ptr[32] = {NULL};
    int i, j;
    UINT32 size = 1201;

    j = 0;
    for (i = 0; i < 32; ++i)
    {
        if ((ptr[i] = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, size, 8)) != NULL)
        {
            j++;
            size += (UINT8)rand();
        }
        else
        {
            size -= (UINT8)rand();
        }
    }
    printf("j: %d\n", j);
    for (i = 0; i < 32; ++i)
    {
        if (ptr[i] != NULL)
        {
            TEST_ASSERT_EQUALS(LOS_OK, LOS_MemFree(m_aucSysMem0, ptr[i]));
            ptr[i] = NULL;
        }
    }
}

void TestLosHeap::test_LOS_MemRealloc(void)
{
    UINT8 *pData = NULL;

    pData = (UINT8 *)LOS_MemRealloc(NULL, NULL, 1);
    TEST_ASSERT(pData == NULL);

    pData = (UINT8 *)LOS_MemAlloc(m_aucSysMem0, 10);
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 8); // cutdown
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 10); // expand
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 0); // free
    TEST_ASSERT(pData == NULL);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 10, 8);
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 168); // cutdown
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 0); // free
    TEST_ASSERT(pData == NULL);

    pData = (UINT8 *)LOS_MemAllocAlign(m_aucSysMem0, 256, 8);
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 168); // cutdown
    TEST_ASSERT(pData != NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, pData, 0); // free
    TEST_ASSERT(pData == NULL);
    pData = (UINT8 *)LOS_MemRealloc(m_aucSysMem0, NULL, 20);
    TEST_ASSERT(pData != NULL);
}

void TestLosHeap::test_LOS_MemFree(void)
{
    TEST_ASSERT_EQUALS(LOS_NOK, LOS_MemFree(NULL, NULL));
    TEST_ASSERT_EQUALS(LOS_NOK, LOS_MemFree(m_aucSysMem0, NULL));
}

#if (LOSCFG_MEM_STATISTICS == YES)
static void __print_heap_statistics (mem_stat_t * stat)
    {
    printf ("\nheap statistics:\n");
    printf ("free_chunks:          %u\n", stat->free_chunks);
    printf ("free_size:            %u\n", stat->free_size);
    printf ("busy_chunks:          %u\n", stat->busy_chunks);
    printf ("busy_size:            %u\n", stat->busy_size);
    printf ("max_busy_size:        %u\n", stat->max_busy_size);
    printf ("max_free_size:        %u\n", stat->max_free_size);
    printf ("cum_allocated:        %llu\n", stat->cum_allocated);
    printf ("cum_size_allocated:   %llu\n", stat->cum_size_allocated);
    printf ("cum_freed:            %llu\n", stat->cum_freed);
    printf ("cum_size_freed:       %llu\n", stat->cum_size_freed);
    }
#endif

void TestLosHeap::test_LOS_MemStatisticsGet(void)
{
    printf("LOS_MemStatisticsGet() is not implemented!\n");
    printf("Will use the heap_stat_get()\n");
#if (LOSCFG_MEM_STATISTICS == YES)
    mem_stat_t stat;
    TEST_ASSERT_EQUALS(LOS_NOK, heap_stat_get((heap_t *)m_aucSysMem0, NULL));
    TEST_ASSERT_EQUALS(LOS_OK, heap_stat_get((heap_t *)m_aucSysMem0, &stat));
    __print_heap_statistics(&stat);
#endif
    printf("\ndump heap: hide chunck:\n");
    LOS_MemInfo(m_aucSysMem0, false);
    printf("\ndump heap: show chunck:\n");
    LOS_MemInfo(m_aucSysMem0, true);
}

void TestLosHeap::test_heap(void)
{
    TEST_ASSERT_EQUALS(-1, heap_init(NULL));
    TEST_ASSERT_EQUALS(-1, heap_add(NULL, NULL, 0));
    TEST_ASSERT_EQUALS(-1, heap_free(NULL, NULL));
    TEST_ASSERT_EQUALS(0, heap_free((heap_t *)m_aucSysMem0, NULL));
}
