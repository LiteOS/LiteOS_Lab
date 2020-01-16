#ifndef __TEST_COAP_CORE_H__
#define __TEST_COAP_CORE_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestCoapCore : public Test::Suite
{
public:
    TestCoapCore();
    ~TestCoapCore(){};

    void test_litecoap_parse_header(void);
    void test_litecoap_parse_token(void);
    void test_litecoap_parse_one_option(void);
    void test_litecoap_parse_opts_payload(void);
    void test_litecoap_build_byte_stream(void);
    void test_litecoap_add_option_to_list(void);
    void test_litecoap_free_option(void);
    void test_litecoap_add_token(void);
    void test_litecoap_add_option(void);
    void test_litecoap_add_paylaod(void);
    void test_litecoap_new_msg(void);
    void test_litecoap_send_back(void);
    void test_litecoap_register_handler(void);
    void test_litecoap_addto_resndqueue(void);
    void test_litecoap_discard_resndqueue(void);
    void test_litecoap_addto_sndqueue(void);
    void test_litecoap_add_resource(void);
    void test_litecoap_option_check_critical(void);
    void test_litecoap_handle_request(void);
    void test_litecoap_handle_msg(void);
    void test_litecoap_read(void);
    void test_litecoap_send(void);

};

#endif

