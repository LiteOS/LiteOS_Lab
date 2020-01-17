#include "test_coap_al.h"
#include "cpp_stub.h"

#include <cstring>

extern "C"
{
    #include "coap_core.h"
    #include <malloc.h>
    #include "litecoap.h"
    #include "litecoap_port.h"
    #include "string.h"
    #include "osal.h"
    #include <coap_al.h>

    extern int osal_init(void);
    extern int coap_install_litecoap();
    extern int litecoap_handle_request(coap_context_t *ctx, coap_msg_t *rcvmsg);

    extern int coap_al_init(coap_al_initpara_t *initparam);
    extern int coap_al_deinit(void *handle);
    extern void *coap_al_add_option(coap_al_optpara_t *optparam);
    extern void *coap_al_new_request(coap_al_reqpara_t *reqparam);
    extern int coap_al_send(coap_al_sndpara_t *sndparam);
    extern int coap_al_recv(coap_al_rcvpara_t *rcvparam);
    extern int coap_al_install(coap_al_op_t *op);
    extern int coap_al_uninstall();

    extern void *litecoap_new_resource_stub(char *ipaddr, unsigned short port, void *ssl)
    {
        return NULL;
    }

    extern coap_context_t *litecoap_malloc_context_stub(void *res)
    {
        return NULL;
    }

    extern int litecoap_register_handler_stub(coap_context_t *ctx, msghandler func)
    {
        return -1;
    }
}

extern void my_cmd_fun(void *msg, int len);

coap_al_initpara_t initpara;

static int my__init(coap_al_initpara_t *initparam)
{
    return 0;
}

static int my__deinit(void *handle)
{
    return 0;
}

void* my__add_opt(coap_al_optpara_t *optparam)
{
    return litecoap_malloc(sizeof(coap_option_t));
}

void* my__request(coap_al_reqpara_t *reqparam)
{
    return litecoap_malloc(sizeof(coap_msg_t));
}

static int my__send(coap_al_sndpara_t *sndparam)
{
    return 0;
}

static int my__recv(coap_al_rcvpara_t *rcvparam)
{
    return 0;
}

TestCoapAL::TestCoapAL()
{
    osal_init();

    TEST_ADD(TestCoapAL::test_coap_al_install);
    TEST_ADD(TestCoapAL::test_coap_al_init);
    TEST_ADD(TestCoapAL::test_coap_al_add_option);
    TEST_ADD(TestCoapAL::test_coap_al_new_request);
    TEST_ADD(TestCoapAL::test_coap_al_send);
    TEST_ADD(TestCoapAL::test_coap_al_recv);
    TEST_ADD(TestCoapAL::test_coap_al_deinit);
    TEST_ADD(TestCoapAL::test_coap_al_uninstall);
}

void TestCoapAL::test_coap_al_init(void)
{
    memset(&initpara,0,sizeof(coap_al_initpara_t));
    initpara.address = (char *)("192.168.1.100");
    initpara.port = 5683;
    initpara.dealer = my_cmd_fun;
    initpara.psk = (const unsigned char*)("010203040506");
    initpara.psklen = 12;
    initpara.pskid = (const unsigned char*)("1");

    // stub test
    Stub sb;
    sb.set(litecoap_new_resource, litecoap_new_resource_stub);

    TEST_ASSERT(0 != coap_al_init(&initpara));

    sb.reset(litecoap_new_resource);

    sb.set(litecoap_malloc_context, litecoap_malloc_context_stub);

    TEST_ASSERT(0 != coap_al_init(&initpara));

    sb.reset(litecoap_malloc_context);

    sb.set(litecoap_register_handler, litecoap_register_handler_stub);

    TEST_ASSERT(0 != coap_al_init(&initpara));

    sb.reset(litecoap_register_handler);

    TEST_ASSERT(0 == coap_al_init(&initpara));
}

void TestCoapAL::test_coap_al_deinit(void)
{
    TEST_ASSERT(0 == coap_al_deinit(initpara.ctx));
}

void TestCoapAL::test_coap_al_add_option(void)
{
    coap_al_optpara_t *optpara = NULL;
    unsigned char res[3]={'t'};
	void *opts = NULL;

    optpara = (coap_al_optpara_t *)litecoap_malloc(sizeof(coap_al_optpara_t));
    TEST_ASSERT(NULL !=  optpara);

    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_URI_PATH;
    optpara->data = (char *)res;
    optpara->len = 1;
    opts = coap_al_add_option(optpara);
    TEST_ASSERT(NULL != opts);

    litecoap_free(optpara);
    litecoap_free_option((coap_option_t *)opts);
}

static void __test_coap_al_new_request(unsigned char code)
{
    unsigned char res[3]={'t'};
    unsigned char res1[3]={'d'};
	void *opts = NULL;
    coap_al_optpara_t *optpara = NULL;
    coap_al_reqpara_t *reqpara = NULL;

    optpara = (coap_al_optpara_t *)litecoap_malloc(sizeof(coap_al_optpara_t));
    // TEST_ASSERT(NULL !=  optpara);

    reqpara = (coap_al_reqpara_t *)litecoap_malloc(sizeof(coap_al_reqpara_t));
    // TEST_ASSERT(NULL !=  optpara);

    char *buf = (char *)litecoap_malloc(256);
    // TEST_ASSERT(NULL !=  buf);
    strcpy(buf, "hello");

    memset(optpara,0,sizeof(coap_al_optpara_t));
    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_URI_PATH;
    optpara->data = (char *)res;
    optpara->len = 1;
    opts = coap_al_add_option(optpara);

    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_URI_PATH;
    optpara->data = (char *)res1;
    optpara->len = 1;
    opts = coap_al_add_option(optpara);

    memset(reqpara,0,sizeof(coap_al_reqpara_t));
    reqpara->ctx = initpara.ctx;
    reqpara->msgtype = COAP_AL_MESSAGE_CON;
    reqpara->code = code;
    reqpara->optlst = opts;
    reqpara->payload = (unsigned char*)buf;
    reqpara->len = strlen(buf);

    coap_msg_t *msg = (coap_msg_t *)coap_al_new_request(reqpara);

    // TEST_ASSERT(NULL != msg);
    msg->optcnt = 2;

    // test method handle_cmd_request
    litecoap_handle_request((coap_context_t *)(initpara.ctx), msg);
    litecoap_delete_msg(msg);

    litecoap_free(optpara);
    litecoap_free(reqpara);
    litecoap_free(buf);
}

void TestCoapAL::test_coap_al_new_request(void)
{
    __test_coap_al_new_request(COAP_AL_REQUEST_POST);
    __test_coap_al_new_request(COAP_AL_REQUEST_GET);
}

void TestCoapAL::test_coap_al_send(void)
{
    unsigned char lifetime = 0;
	void *opts = NULL;
    coap_al_optpara_t *optpara = NULL;
    coap_al_reqpara_t *reqpara = NULL;

    optpara = (coap_al_optpara_t *)litecoap_malloc(sizeof(coap_al_optpara_t));
    TEST_ASSERT(NULL !=  optpara);

    reqpara = (coap_al_reqpara_t *)litecoap_malloc(sizeof(coap_al_reqpara_t));
    TEST_ASSERT(NULL !=  reqpara);

    char *buf = (char *)litecoap_malloc(32);
    TEST_ASSERT(NULL !=  buf);
    strcpy(buf, "hello");

    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_OBSERVE;
    optpara->data = (char *)&lifetime;
    optpara->len = 1;
    opts = coap_al_add_option(optpara);

    memset(reqpara,0,sizeof(coap_al_reqpara_t));
    reqpara->ctx = initpara.ctx;
    reqpara->msgtype = COAP_AL_MESSAGE_NON;
    reqpara->code = COAP_AL_RESP_CODE(205);
    reqpara->optlst = opts;
    reqpara->payload = (unsigned char*)buf;
    reqpara->len = strlen(buf);

    coap_msg_t *msg = (coap_msg_t *)coap_al_new_request(reqpara);

    TEST_ASSERT(NULL != msg);

    coap_al_sndpara_t sndpara;
    memset(&sndpara,0,sizeof(coap_al_sndpara_t));
    sndpara.handle = initpara.ctx;
    sndpara.msg = msg;
    int ret = coap_al_send(&sndpara);

    // not connect case
    TEST_ASSERT(0 != ret);

    litecoap_free(optpara);
    litecoap_free(reqpara);
    litecoap_free(buf);

}

void TestCoapAL::test_coap_al_recv(void)
{
    coap_al_rcvpara_t rcvpara;
    memset(&rcvpara,0,sizeof(coap_al_rcvpara_t));
    rcvpara.ctx = initpara.ctx;
    int ret = coap_al_recv(&rcvpara);

    // not connect case
    TEST_ASSERT(0 != ret);
}

void TestCoapAL::test_coap_al_install(void)
{
    TEST_ASSERT(0 == coap_al_uninstall());

    coap_al_op_t litecoap_op =
    {
    	.init = my__init,
		.deinit = my__deinit,
		.add_opt = my__add_opt,
        .request = my__request,
        .send = my__send,
        .recv = my__recv,
    };

    TEST_ASSERT(0 == coap_al_install(&litecoap_op));
    TEST_ASSERT(0 == coap_al_uninstall());
    TEST_ASSERT(0 == coap_install_litecoap());
}

void TestCoapAL::test_coap_al_uninstall(void)
{
    TEST_ASSERT(0 == coap_al_uninstall());
}
