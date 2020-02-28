#ifndef __TEST_LITECOAP_PORT_H__
#define __TEST_LITECOAP_PORT_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestLiteCoapPort : public Test::Suite
{
public:
    TestLiteCoapPort();
    ~TestLiteCoapPort();

    void test_coap_install_litecoap(void);
    void test_handle_coap_response(void);
    void test___add_opt(void);
    void test___request(void);
};

#endif

