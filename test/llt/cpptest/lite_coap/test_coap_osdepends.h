#ifndef __TEST_COAP_OSDEPEND_H__
#define __TEST_COAP_OSDEPEND_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestCoapOsDepends : public Test::Suite
{
public:
    TestCoapOsDepends();
    ~TestCoapOsDepends(){};

    void test_litecoap_generate_token(void);
    void test_litecoap_new_resource(void);
    void test_litecoap_malloc_context(void);
};

#endif

