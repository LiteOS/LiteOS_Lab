#include "test_litecoap_port.h"

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
    extern int handle_coap_response(struct _coap_context_t *ctx, coap_msg_t *msg);
    extern void* __add_opt(coap_al_optpara_t *optparam);
    extern void* __request(coap_al_reqpara_t *reqparam);
    extern int coap_al_uninstall();
}

void my_cmd_fun(void *msg, int len)
{
}

void TestLiteCoapPort::test_coap_install_litecoap(void)
{
    TEST_ASSERT(0 == coap_install_litecoap());
}

TestLiteCoapPort::TestLiteCoapPort()
{
    osal_init();

    TEST_ADD(TestLiteCoapPort::test_coap_install_litecoap);
    TEST_ADD(TestLiteCoapPort::test_handle_coap_response);
    TEST_ADD(TestLiteCoapPort::test___add_opt);
    TEST_ADD(TestLiteCoapPort::test___request);
}

TestLiteCoapPort::~TestLiteCoapPort()
{
    coap_al_uninstall();
}

void TestLiteCoapPort::test_handle_coap_response(void)
{
    coap_msg_t msg;
    struct _coap_context_t ctx;

    memset(&msg, 0, sizeof(coap_msg_t));

    // condition 1
    msg.head.t = COAP_MESSAGE_CON;
    msg.head.code = COAP_REQUEST_GET;

    msg.option = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    TEST_ASSERT(NULL != msg.option);

    msg.option->optnum = COAP_OPTION_OBSERVE;

    msg.tok = (coap_token_t *)litecoap_malloc(sizeof(coap_token_t));
    TEST_ASSERT(NULL !=  msg.tok);
    memset(msg.tok, 0, sizeof(coap_token_t));

    int tklen = 8;

    msg.tok->token = (unsigned char *)litecoap_malloc(tklen);
    TEST_ASSERT(NULL !=  msg.tok->token);

    memset(msg.tok->token, 0, tklen);
    msg.tok->tklen = tklen;
    msg.head.tkl = 8;

    TEST_ASSERT(0 == handle_coap_response(&ctx, &msg));
    TEST_ASSERT(0 == handle_coap_response(&ctx, &msg));

    // condition 2
    msg.head.t = COAP_MESSAGE_RST;

    TEST_ASSERT(0 == handle_coap_response(&ctx, &msg));

    // COAP_MESSAGE_ACK == msg->head.t) && (0xa0 == msg->head.code
    msg.head.t = COAP_MESSAGE_ACK;
    msg.head.code = 0xa0;

    TEST_ASSERT(0 == handle_coap_response(&ctx, &msg));

    // COAP_MESSAGE_ACK == msg->head.t) && (0xa0 == msg->head.code
    msg.head.t = COAP_MESSAGE_ACK;
    msg.head.code = 0x44;

    TEST_ASSERT(0 == handle_coap_response(&ctx, &msg));


    // COAP_MESSAGE_ACK == msg->head.t) && (0xa0 == msg->head.code
    msg.head.t = -1;

    TEST_ASSERT(0 == handle_coap_response(&ctx, &msg));

    litecoap_free(msg.option);
    litecoap_free(msg.tok->token);
    litecoap_free(msg.tok);
}

void TestLiteCoapPort::test___add_opt(void)
{
    coap_al_optpara_t *optpara = NULL;
    unsigned char res[3] = {'t'};
    void *opts = NULL;

    coap_al_optpara_t optparam;
    memset(&optparam, 0, sizeof(optparam));
    TEST_ASSERT(NULL == __add_opt(&optparam));

    optpara = (coap_al_optpara_t *)litecoap_malloc(sizeof(coap_al_optpara_t));
    TEST_ASSERT(NULL !=  optpara);

    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_URI_PATH;
    optpara->data = (char *)res;
    optpara->len = 1;
    opts = __add_opt(optpara);
    TEST_ASSERT(NULL != opts);

    litecoap_free(optpara);
    litecoap_free_option((coap_option_t *)opts);
}

void TestLiteCoapPort::test___request(void)
{
    unsigned char lifetime = 0;
    void *opts = NULL;
    coap_al_optpara_t *optpara = NULL;
    coap_al_reqpara_t *reqpara = NULL;

    coap_install_litecoap();
    coap_al_initpara_t initpara;
    memset(&initpara, 0, sizeof(coap_al_initpara_t));
    initpara.address = (char *)("192.168.1.100");
    initpara.port = 5683;
    initpara.dealer = my_cmd_fun;
    initpara.psk = (const unsigned char*)("010203040506");
    initpara.psklen = 12;
    initpara.pskid = (const unsigned char*)("1");
    TEST_ASSERT(0 == coap_al_init(&initpara));

    optpara = (coap_al_optpara_t *)litecoap_malloc(sizeof(coap_al_optpara_t));
    TEST_ASSERT(NULL !=  optpara);

    reqpara = (coap_al_reqpara_t *)litecoap_malloc(sizeof(coap_al_reqpara_t));
    TEST_ASSERT(NULL !=  optpara);

    char *buf = (char *)litecoap_malloc(256);
    TEST_ASSERT(NULL !=  buf);
    strcpy(buf, "hello");

    optpara->head = opts;
    optpara->opt_num = COAP_AL_OPTION_OBSERVE;
    optpara->data = (char *)&lifetime;
    optpara->len = 1;
    opts = __add_opt(optpara);

    memset(reqpara, 0, sizeof(coap_al_reqpara_t));
    reqpara->ctx = initpara.ctx;
    reqpara->msgtype = COAP_AL_MESSAGE_NON;
    reqpara->code = COAP_AL_RESP_CODE(205);
    reqpara->optlst = opts;
    reqpara->payload = (unsigned char*)buf;
    reqpara->len = strlen(buf);

    void *msg = __request(reqpara);

    TEST_ASSERT(NULL != msg);

    litecoap_free(optpara);
    litecoap_free(reqpara);
    litecoap_free(buf);
    litecoap_delete_msg((coap_msg_t *)msg);
    coap_al_deinit(initpara.ctx);
}
