#ifndef __TEST_COAP_AL_H__
#define __TEST_COAP_AL_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestCoapAL : public Test::Suite
{
public:
    TestCoapAL();
    ~TestCoapAL(){};

    void test_coap_al_init(void);
    void test_coap_al_deinit(void);
    void test_coap_al_add_option(void);
    void test_coap_al_new_request(void);
    void test_coap_al_send(void);
    void test_coap_al_recv(void);
    void test_coap_al_install(void);
    void test_coap_al_uninstall(void);
};

#endif

