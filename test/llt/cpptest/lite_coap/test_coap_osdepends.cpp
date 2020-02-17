#include "test_coap_osdepends.h"


extern "C"
{
    #include "coap_core.h"
    #include <malloc.h>
    #include "litecoap.h"
    #include "litecoap_port.h"
    #include "string.h"
    #include "osal.h"

    extern int osal_init(void);
    extern int link_tcpip_init( void);
    extern int litecoap_generate_token(unsigned char *token);
    extern coap_context_t *litecoap_malloc_context(void *res);
}

void TestCoapOsDepends::test_litecoap_generate_token(void)
{
    unsigned char *token = (unsigned char *)litecoap_malloc(5);

    TEST_ASSERT(8 == litecoap_generate_token(token));
    litecoap_free(token);
}

void TestCoapOsDepends::test_litecoap_new_resource(void)
{
    const char *ipaddr1 = "192.168.1.100";
    const char *ipaddr2 = "192.168.1.256";
    void *tmp;

    TEST_ASSERT(NULL == litecoap_new_resource(NULL, 0, NULL));
    TEST_ASSERT(NULL == litecoap_new_resource((char *)ipaddr2, 0, NULL));

    TEST_ASSERT(tmp = litecoap_new_resource((char *)ipaddr1, 8080, NULL));
    osal_free(tmp);
}

void TestCoapOsDepends::test_litecoap_malloc_context(void)
{

    coap_context_t *tmp;
    void *res = osal_malloc(sizeof(20));;
    TEST_ASSERT(NULL == litecoap_malloc_context(NULL));
    TEST_ASSERT(tmp = litecoap_malloc_context(res));
    litecoap_free_context(NULL); //for litecoap_free_context

    send_queue_t *sndque = (send_queue_t *)osal_malloc(sizeof(send_queue_t));
    TEST_ASSERT(NULL != sndque);
    tmp->sndque = sndque;

    litecoap_free_context(tmp);

    // stub test

}

TestCoapOsDepends::TestCoapOsDepends()
{
    osal_init();
    link_tcpip_init();

    TEST_ADD(TestCoapOsDepends::test_litecoap_generate_token);
    TEST_ADD(TestCoapOsDepends::test_litecoap_new_resource);
    TEST_ADD(TestCoapOsDepends::test_litecoap_malloc_context);
}
