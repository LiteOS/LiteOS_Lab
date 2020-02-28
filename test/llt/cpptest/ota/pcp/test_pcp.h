#ifndef __TEST_PCP_H__
#define __TEST_PCP_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestPcp : public Test::Suite
{
public:
    TestPcp();
    ~TestPcp();
    void test_ota_pcp_init(void);
    void test_pcp_msg_push(void);


};



#endif /* __TEST_PCP_H__ */


