#ifndef __TEST_QUEUE_H__
#define __TEST_QUEUE_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestQueue : public Test::Suite
{
public:
    TestQueue();
    ~TestQueue();

    void test_queue_create(void);
    void test_queue_push(void);
    void test_queue_pop(void);
    void test_queue_delete(void);
    void test_queue_demo(void);
    void test_queue_stub(void);
};



#endif /* __TEST_QUEUE_H__ */
