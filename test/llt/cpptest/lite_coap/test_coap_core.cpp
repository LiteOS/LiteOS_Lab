#include "test_coap_core.h"


extern "C"
{
    #include "coap_core.h"
    #include <malloc.h>
    #include "litecoap.h"
    #include "string.h"
    #include "osal.h"

    extern int osal_init(void);
    extern int litecoap_sal_send(void *handle, char *buf, int size);
    extern int litecoap_sal_read(void *handle, char *buf, int size);
    extern int handle_coap_response(struct _coap_context_t *ctx, coap_msg_t *msg);
    int resHandler(coap_msg_t *rcvmsg, coap_msg_t *outmsg) { return 0; }
    int msgHandler(struct _coap_context_t *ctx, coap_msg_t *msg) { return 0; }
    extern int litecoap_parse_header(coap_msg_t *msg, const unsigned char *buf, int buflen);
    extern int litecoap_parse_token(coap_msg_t *msg, unsigned char *buf, int buflen);
    extern int litecoap_parse_one_option(coap_msg_t *msg, unsigned short *sumdelta,const unsigned char **buf, int buflen);
    extern int litecoap_parse_opts_payload(coap_msg_t *msg, const unsigned char *buf, int buflen);
    extern int litecoap_build_byte_stream(coap_context_t *ctx, coap_msg_t *msg);
    extern int litecoap_send_back(coap_context_t *ctx, coap_msg_t *rcvmsg, unsigned char type);
    extern int litecoap_send_rst(coap_context_t *ctx, coap_msg_t *rcvmsg);
    extern int litecoap_send_ack(coap_context_t *ctx, coap_msg_t *rcvmsg);
    extern int litecoap_register_handler(coap_context_t *ctx, msghandler func);
    extern int litecoap_addto_resndqueue(coap_context_t *ctx, coap_msg_t *msg);
    extern int litecoap_remove_resndqueue(coap_context_t *ctx, coap_msg_t *rcvmsg);
    extern int litecoap_discard_resndqueue(coap_context_t *ctx);
    extern int litecoap_addto_sndqueue(coap_context_t *ctx, coap_msg_t *msg);
    extern int litecoap_remove_sndqueue(coap_context_t *ctx, coap_msg_t *msg);
    extern int litecoap_option_check_critical(coap_msg_t *msg);
    extern int litecoap_handle_request(coap_context_t *ctx, coap_msg_t *rcvmsg);
    extern int litecoap_handle_msg(coap_context_t *ctx, coap_msg_t *msg);
}

void TestCoapCore::test_litecoap_parse_header(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    unsigned char buf[4]={0xC0,0x00,0x00,0x00};
    TEST_ASSERT(-1 == litecoap_parse_header(msg, NULL, 1));
    TEST_ASSERT(-1 == litecoap_parse_header(NULL, buf, 1));
    TEST_ASSERT(-2 == litecoap_parse_header(msg, buf, 1));
    TEST_ASSERT(-3 == litecoap_parse_header(msg, buf, 4));
    buf[0] = 0x40;
    TEST_ASSERT(0 == litecoap_parse_header(msg, buf, 4));
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_parse_token(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    unsigned char buf[4]={0xC0,0x00,0x00,0x00};
    TEST_ASSERT(-1 == litecoap_parse_token(msg, NULL, 1));
    TEST_ASSERT(-1 == litecoap_parse_token(NULL, buf, 1));
    msg->head.tkl = 0;
    TEST_ASSERT(0 == litecoap_parse_token(msg, buf, 1));
    msg->head.tkl = 8;
    TEST_ASSERT(-2 == litecoap_parse_token(msg, buf, 1));
    msg->head.tkl = 9;
    TEST_ASSERT(-4 == litecoap_parse_token(msg, buf, 20));
    msg->head.tkl = 8;
    TEST_ASSERT(0 == litecoap_parse_token(msg, buf, 20));
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_parse_one_option(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    unsigned char *buf = NULL;
    unsigned short sumdelta = 0;
    buf = (unsigned char *)litecoap_malloc(20);
    unsigned char *p = buf;

    TEST_ASSERT(-1 == litecoap_parse_one_option(msg, NULL, (const unsigned char **)&p, 0));
    p = buf;
    TEST_ASSERT(-2 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 0));
    p = buf;
    p[0] = 0xdd;
    TEST_ASSERT(-2 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 1));
    p = buf;
    TEST_ASSERT(-2 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 2));
    p = buf;
    TEST_ASSERT(0 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 20));
    if (msg->option) {
        litecoap_free_option(msg->option);
        msg->option = NULL;
    }
    msg->option = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    p = buf;
    TEST_ASSERT(0 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 20));
    p = buf;
    p[0] = 0xee;
    TEST_ASSERT(-2 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 2));
    p = buf;
    TEST_ASSERT(-2 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 3));
    p = buf;
    TEST_ASSERT(-6 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 20));
    p = buf;
    p[0] = 0xff;
    TEST_ASSERT(-5 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 20));
    p = buf;
    p[0] = 0xdf;
    TEST_ASSERT(-6 == litecoap_parse_one_option(msg, &sumdelta, (const unsigned char **)&p, 20));

    litecoap_free(buf);
    litecoap_delete_msg(msg);

}

void TestCoapCore::test_litecoap_parse_opts_payload(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    unsigned char *buf = (unsigned char *)litecoap_malloc(40);
    TEST_ASSERT(-1 == litecoap_parse_opts_payload(msg, buf, 4));
    msg->head.tkl = 6;
    TEST_ASSERT(-7 == litecoap_parse_opts_payload(msg, buf, 8));
    buf[11] = 0xdf;
    TEST_ASSERT(-6 == litecoap_parse_opts_payload(msg, buf, 30));
    buf[11] = 0xdd;
    buf[27] = 0xff;
    TEST_ASSERT(0 == litecoap_parse_opts_payload(msg, buf, 30));
    if (msg->payload) {
        litecoap_free(msg->payload);
        msg->payload = NULL;
    }
    buf[27] = 0xff;
    TEST_ASSERT(0 == litecoap_parse_opts_payload(msg, buf, 28));

    litecoap_free(buf);
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_build_byte_stream(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    TEST_ASSERT(-1 == litecoap_build_byte_stream(NULL, NULL));
    TEST_ASSERT(-9 == litecoap_build_byte_stream(ctx, msg));

    ctx->sndbuf.buf = (unsigned char *)litecoap_malloc(1024);
    ctx->sndbuf.len = 10;

    msg->option =(coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    msg->option->optlen = 0;
    msg->option->optnum = 1;
    msg->payloadlen = 1;
    TEST_ASSERT(-10 == litecoap_build_byte_stream(ctx, msg));

    msg->tok = (coap_token_t *)litecoap_malloc(sizeof(coap_token_t));
    msg->tok->tklen = 1;
    ctx->sndbuf.len = 500;
    msg->tok->token = (unsigned char *)litecoap_malloc(10);
    msg->payload = (unsigned char *)litecoap_malloc(10);
    msg->payloadlen = 10;
    msg->payloadmarker = 0xdd;
    msg->head.tkl = 0x01;
    coap_option_t * option_2 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_2->optnum = 15;
    option_2->optlen = 15;
    option_2->value = (unsigned char *)litecoap_malloc(option_2->optlen);
    coap_option_t * option_3 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_3->optnum = 315;
    option_3->optlen = 315;
    option_3->value = (unsigned char *)litecoap_malloc(option_3->optlen);
    coap_option_t * option_4 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_4->optnum = 10;
    option_4->optlen = 10;
    option_4->value = (unsigned char *)litecoap_malloc(option_4->optlen);
    msg->option->next = option_2;
    option_2->next = option_3;
    option_3->next = option_4;

    //   offset  len1   len2   len3    len4    msg->payloadlen
    //      5      2     18     320      0       10
    TEST_ASSERT(355 == litecoap_build_byte_stream(ctx, msg));


    litecoap_delete_msg(msg);
    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_add_option_to_list(void)
{
    TEST_ASSERT(NULL == litecoap_add_option_to_list(NULL, 0, NULL, 0));

    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    msg->option = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    msg->option->optlen = 0;
    msg->option->optnum = 11;
    coap_option_t * head = NULL;
    char *value = (char *)litecoap_malloc(10);
    memset(value, 0x09, 10);
    head = litecoap_add_option_to_list(NULL, 0, value, 10); // value
    TEST_ASSERT(!(strncmp(value, (char *)head->value, 10)));
    litecoap_free_option(head);
    head = NULL;

    head = litecoap_add_option_to_list(msg->option, 10, value, 10); // value
    TEST_ASSERT(!(strncmp(value, (char *)head->value, 10)));
    head->next =NULL;
    litecoap_free_option(head);
    head = NULL;

    coap_option_t * option_2 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_2->optnum = 14;
    option_2->optlen = 14;
    option_2->value = (unsigned char *)litecoap_malloc(option_2->optlen);
    coap_option_t * option_3 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_3->optnum = 20;
    option_3->optlen = 20;
    option_3->value = (unsigned char *)litecoap_malloc(option_3->optlen);
    coap_option_t * option_4 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_4->optnum = 10;
    option_4->optlen = 10;
    option_4->value = (unsigned char *)litecoap_malloc(option_4->optlen);
    msg->option->next = option_2;
    option_2->next = option_3;
    option_3->next = option_4;
    head = litecoap_add_option_to_list(msg->option, 15, value, 10); // temp->next->next = new
    TEST_ASSERT(!(strncmp(value, (char *)head->next->next->value, 10)));

    litecoap_free(value);
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_free_option(void)
{
    coap_option_t * head = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    TEST_ASSERT(0 == litecoap_free_option(head));
    TEST_ASSERT(-1 == litecoap_free_option(NULL));
}

void TestCoapCore::test_litecoap_add_token(void)
{
    TEST_ASSERT(-1 == litecoap_add_token(NULL,NULL,0));

    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    TEST_ASSERT(-1 == litecoap_add_token(msg,NULL,9));

    msg->tok = NULL;
    char *tok = (char *)litecoap_malloc(8);
    TEST_ASSERT(0 == litecoap_add_token(msg,tok,8));

    litecoap_free(tok);
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_add_option(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    coap_option_t * head = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    TEST_ASSERT(-1 == litecoap_add_option(NULL, head));
    TEST_ASSERT(-1 == litecoap_add_option(msg, NULL));
    TEST_ASSERT(0 == litecoap_add_option(msg, head));
    TEST_ASSERT(msg->option == head);
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_add_paylaod(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    char *payload = (char *)litecoap_malloc(8);
    memset(payload, 0x09, 8);
    TEST_ASSERT(-1 == litecoap_add_paylaod(msg, NULL, 0));
    TEST_ASSERT(-1 == litecoap_add_paylaod(msg, payload, 0));
    TEST_ASSERT(0 == litecoap_add_paylaod(msg, payload, 8));
    TEST_ASSERT(!(strncmp(payload, (char *)msg->payload, 8)));

    litecoap_free(payload);
    litecoap_delete_msg(msg);
}

void TestCoapCore::test_litecoap_new_msg(void)
{
    coap_msg_t *msg = NULL;
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    unsigned char msgtype = 0x04;
    unsigned char code = 0x00;
    coap_option_t *optlist = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    unsigned char *payload = (unsigned char *)litecoap_malloc(8);
    memset(payload, 0x09, 8);
    int payloadlen = 8;
    TEST_ASSERT(NULL == litecoap_new_msg(NULL, msgtype, code, optlist, payload, payloadlen));
    TEST_ASSERT(NULL == litecoap_new_msg(ctx, msgtype, code, optlist, payload, payloadlen));
    msgtype = 0x03;
    ctx->msgid = 0x1122;
    msg = litecoap_new_msg(ctx, msgtype, code, optlist, payload, payloadlen);
    TEST_ASSERT(!(strncmp((char *)payload, (char *)msg->payload, 8)));

    litecoap_delete_msg(msg);
    litecoap_free_context(ctx);
    litecoap_free(payload);
}

void TestCoapCore::test_litecoap_send_back(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    unsigned char type = 0x04;

    TEST_ASSERT(-1 == litecoap_send_back(NULL, NULL, type));
    TEST_ASSERT(-11 == litecoap_send_back(ctx, msg, type));

    msg->option = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    msg->option->optlen = 0;
    msg->option->optnum = 11;
    ctx->sndbuf.buf = (unsigned char *)litecoap_malloc(1024);

    msg->tok = (coap_token_t *)litecoap_malloc(sizeof(coap_token_t));
    msg->tok->tklen = 1;
    ctx->sndbuf.len = 500;
    msg->tok->token = (unsigned char *)litecoap_malloc(10);
    msg->payload = (unsigned char *)litecoap_malloc(10);
    msg->payloadlen = 10;
    msg->payloadmarker = 0xdd;
    msg->head.tkl = 0x01;
    coap_option_t * option_2 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    option_2->optnum = 15;
    option_2->optlen = 15;
    option_2->value = (unsigned char *)litecoap_malloc(option_2->optlen);
    msg->option->next = option_2;
    type = 0x02;
    ctx->netops = (udp_ops *)litecoap_malloc(sizeof(udp_ops));
    ctx->netops->network_send = litecoap_sal_send;
    TEST_ASSERT(0 == litecoap_send_back(ctx, msg, type));
    TEST_ASSERT(0 == litecoap_send_rst(ctx, msg));
    TEST_ASSERT(0 == litecoap_send_ack(ctx, msg));

    litecoap_delete_msg(msg);
    litecoap_free(ctx->netops);
    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_register_handler(void)
{
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    TEST_ASSERT(-1 == litecoap_register_handler(NULL, handle_coap_response));
    TEST_ASSERT(0 == litecoap_register_handler(ctx, handle_coap_response));

    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_addto_resndqueue(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    memset(msg, 0, sizeof(coap_msg_t));
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));

    TEST_ASSERT(-1 == litecoap_addto_resndqueue(NULL, msg));
    TEST_ASSERT(-1 == litecoap_addto_resndqueue(ctx, NULL));
    ctx->resndque = (send_queue_t *)litecoap_malloc(sizeof(send_queue_t));
    coap_msg_t *msg2 = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    ctx->resndque->msg = msg2;
    TEST_ASSERT(0 == litecoap_addto_resndqueue(ctx, msg));

    TEST_ASSERT(-1 == litecoap_remove_resndqueue(NULL, msg));
    TEST_ASSERT(-1 == litecoap_remove_resndqueue(ctx, NULL));

    msg2->head.msgid[0] = 0x11;
    msg2->head.msgid[1] = 0x36;
    TEST_ASSERT(0 == litecoap_remove_resndqueue(ctx, msg2));
    TEST_ASSERT(0 == litecoap_remove_resndqueue(ctx, msg));

    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_discard_resndqueue(void)
{
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    ctx->resndque = (send_queue_t *)litecoap_malloc(sizeof(send_queue_t));
    ctx->resndque->next = (send_queue_t *)litecoap_malloc(sizeof(send_queue_t));

    TEST_ASSERT(-1 == litecoap_discard_resndqueue(NULL));
    TEST_ASSERT(0 == litecoap_discard_resndqueue(ctx));

    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_addto_sndqueue(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    ctx->sndque = (send_queue_t *)litecoap_malloc(sizeof(send_queue_t));
    coap_msg_t *msg2 = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    ctx->sndque->msg = msg2;

    TEST_ASSERT(-1 == litecoap_addto_sndqueue(NULL, msg));
    TEST_ASSERT(-1 == litecoap_addto_sndqueue(ctx, NULL));
    TEST_ASSERT(0 == litecoap_addto_sndqueue(ctx, msg));

    TEST_ASSERT(-1 == litecoap_remove_sndqueue(NULL, msg));
    TEST_ASSERT(-1 == litecoap_remove_sndqueue(ctx, NULL));
    TEST_ASSERT(0 == litecoap_remove_sndqueue(ctx, msg2));
    litecoap_free(msg);
    TEST_ASSERT(0 == litecoap_remove_sndqueue(ctx, msg));
    litecoap_free(msg2);


    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_add_resource(void)
{
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    coap_res_t *res = (coap_res_t *)litecoap_malloc(sizeof(coap_res_t));

    TEST_ASSERT(-1 == litecoap_add_resource(NULL, res));
    TEST_ASSERT(0 == litecoap_add_resource(ctx, res));

    litecoap_free(res);
    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_option_check_critical(void)
{
    TEST_ASSERT(0 == litecoap_option_check_critical(NULL));
}

void TestCoapCore::test_litecoap_handle_request(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    TEST_ASSERT(-1 == litecoap_handle_request(NULL, msg));
    TEST_ASSERT(-1 == litecoap_handle_request(ctx, NULL));
    ctx->res = (coap_res_t *)litecoap_malloc(sizeof(coap_res_t) * 10);
    ctx->res[0].handler = &resHandler;
    ctx->res[0].method = 0x22;
    msg->head.code = 0x08;
    ctx->res[1].handler = &resHandler;
    ctx->res[1].method = 0x08;
    msg->option = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    coap_option_t * option2 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    coap_option_t * option3 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    coap_option_t * option4 = (coap_option_t *)litecoap_malloc(sizeof(coap_option_t));
    msg->option->optnum = 0;
    option2->optnum = 11;
    option3->optnum = 11;
    option4->optnum = 1;
    msg->option->next = option2;
    option2->next = option3;
    option3->next = option4;

    option2->optlen = 4;
    option2->value = (unsigned char *)litecoap_malloc(option2->optlen);
    memset(option2->value, 0x11, option2->optlen);
    option3->optlen = 4;
    option3->value = (unsigned char *)litecoap_malloc(option3->optlen);
    memset(option3->value, 0x11, option3->optlen);
    msg->optcnt = 4;
    ctx->res[1].path = (coap_res_path_t *)litecoap_malloc(sizeof(coap_res_path_t));

    coap_res_path_t * path = (coap_res_path_t *)litecoap_malloc(sizeof(coap_res_path_t));
    path->elems[0] = (char *)litecoap_malloc(5);
    memset(path->elems[0], 0x11, 4);
    path->elems[0][4] = 0;
    path->elems[1] = (char *)litecoap_malloc(5);
    memset(path->elems[1], 0x11, 4);
    path->elems[1][4] = 0;
    path->count = 2;
    memcpy((void *)ctx->res[1].path, path, sizeof(coap_res_path_t));

    msg->head.t = 0;
    msg->tok = (coap_token_t *)litecoap_malloc(sizeof(coap_token_t));
    msg->tok->tklen = 6;
    msg->tok->token = (unsigned char *)litecoap_malloc(msg->tok->tklen + 4);

    ctx->netops = (udp_ops *)litecoap_malloc(sizeof(udp_ops));
    ctx->netops->network_send = litecoap_sal_send;
    ctx->sndbuf.buf = (unsigned char *)litecoap_malloc(100);
    ctx->sndbuf.len = 10;
    ctx->udpio = litecoap_malloc(100);

    TEST_ASSERT(0 == litecoap_handle_request(ctx, msg));

    msg->head.t = 1;
    TEST_ASSERT(0 == litecoap_handle_request(ctx, msg));
    msg->tok->tklen = 9;
    TEST_ASSERT(-1 == litecoap_handle_request(ctx, msg));


    coap_res_path_t * path2 = (coap_res_path_t *)litecoap_malloc(sizeof(coap_res_path_t));
    path2->elems[0] = (char *)litecoap_malloc(5);
    memset(path2->elems[0], 0x11, 5);
    path2->elems[0][4] = 0;
    path2->elems[1] = (char *)litecoap_malloc(5);
    memset(path2->elems[1], 0x11, 4);
    path2->elems[1][4] = 0;
    path2->count = 2;
    memcpy((void *)ctx->res[1].path, path2, sizeof(coap_res_path_t));

    TEST_ASSERT(-1 == litecoap_handle_request(ctx, msg));

    msg->tok->tklen = 6;
    TEST_ASSERT(0 == litecoap_handle_request(ctx, msg));

    msg->head.t = 0;
    TEST_ASSERT(0 == litecoap_handle_request(ctx, msg));

    coap_res_path_t * path3 = (coap_res_path_t *)litecoap_malloc(sizeof(coap_res_path_t));
    path3->elems[0] = (char *)litecoap_malloc(5);
    memset(path3->elems[0], 0x11, 4);
    path3->elems[0][4] = 0;
    path3->elems[1] = (char *)litecoap_malloc(5);
    memset(path3->elems[1], 0x22, 4);
    path3->elems[1][4] = 0;
    path3->count = 2;
    memcpy((void *)ctx->res[1].path, path3, sizeof(coap_res_path_t));
    TEST_ASSERT(0 == litecoap_handle_request(ctx, msg));

    litecoap_free(path->elems[0]);
    litecoap_free(path->elems[1]);
    litecoap_free(path);
    litecoap_free(path2->elems[0]);
    litecoap_free(path2->elems[1]);
    litecoap_free(path2);
    litecoap_free(path3->elems[0]);
    litecoap_free(path3->elems[1]);
    litecoap_free(path3);
    litecoap_delete_msg(msg);
    litecoap_free((coap_res_path_t *)ctx->res[1].path);
    litecoap_free(ctx->res);
    litecoap_free(ctx->netops);
    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_handle_msg(void)
{
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    ctx->res = (coap_res_t *)litecoap_malloc(sizeof(coap_res_t));
    msg->tok = (coap_token_t *)litecoap_malloc(sizeof(coap_token_t));

    msg->head.t = 2;
    TEST_ASSERT(0 == litecoap_handle_msg(ctx, msg));

    msg->head.t = 3;
    TEST_ASSERT(0 == litecoap_handle_msg(ctx, msg));

    msg->head.t = 0;
    TEST_ASSERT(0 == litecoap_handle_msg(ctx, msg));

    msg->head.t = -1;
    ctx->response_handler = &msgHandler;
    TEST_ASSERT(0 == litecoap_handle_msg(ctx, msg));

    litecoap_delete_msg(msg);
    if (ctx->res) {
        litecoap_free(ctx->res);
        ctx->res = NULL;
    }
    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_read(void)
{
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    TEST_ASSERT(-1 == litecoap_read(NULL));

    ctx->netops = (udp_ops *)litecoap_malloc(sizeof(udp_ops));
    ctx->netops->network_read = litecoap_sal_read;
    ctx->rcvbuf.buf = (unsigned char *)litecoap_malloc(100);
    ctx->rcvbuf.len = 100;
    TEST_ASSERT(-16 == litecoap_read(ctx));

    ctx->udpio = litecoap_malloc(100);
    TEST_ASSERT(-16 == litecoap_read(ctx));

    litecoap_free(ctx->netops);
    litecoap_free_context(ctx);
}

void TestCoapCore::test_litecoap_send(void)
{
    coap_context_t *ctx = (coap_context_t *)litecoap_malloc(sizeof(coap_context_t));
    coap_msg_t *msg = (coap_msg_t *)litecoap_malloc(sizeof(coap_msg_t));

    msg->head.t = 0;
    TEST_ASSERT(-1 == litecoap_send(NULL, NULL));

    ctx->netops = (udp_ops *)litecoap_malloc(sizeof(udp_ops));
    ctx->netops->network_send = litecoap_sal_send;
    TEST_ASSERT(-1 == litecoap_send(ctx, msg));

    litecoap_free(ctx->netops);
    litecoap_free_context(ctx);
}

TestCoapCore::TestCoapCore()
{
    osal_init();

    TEST_ADD(TestCoapCore::test_litecoap_parse_header);
    TEST_ADD(TestCoapCore::test_litecoap_parse_token);
    TEST_ADD(TestCoapCore::test_litecoap_parse_one_option);
    TEST_ADD(TestCoapCore::test_litecoap_parse_opts_payload);
    TEST_ADD(TestCoapCore::test_litecoap_build_byte_stream);
    TEST_ADD(TestCoapCore::test_litecoap_add_option_to_list);
    TEST_ADD(TestCoapCore::test_litecoap_free_option);
    TEST_ADD(TestCoapCore::test_litecoap_add_token);
    TEST_ADD(TestCoapCore::test_litecoap_add_option);
    TEST_ADD(TestCoapCore::test_litecoap_add_paylaod);
    TEST_ADD(TestCoapCore::test_litecoap_new_msg);
    TEST_ADD(TestCoapCore::test_litecoap_send_back);
    TEST_ADD(TestCoapCore::test_litecoap_register_handler);
    TEST_ADD(TestCoapCore::test_litecoap_addto_resndqueue);
    TEST_ADD(TestCoapCore::test_litecoap_discard_resndqueue);
    TEST_ADD(TestCoapCore::test_litecoap_addto_sndqueue);
    TEST_ADD(TestCoapCore::test_litecoap_add_resource);
    TEST_ADD(TestCoapCore::test_litecoap_option_check_critical);
    TEST_ADD(TestCoapCore::test_litecoap_handle_request);
    TEST_ADD(TestCoapCore::test_litecoap_handle_msg);
    TEST_ADD(TestCoapCore::test_litecoap_read);
    TEST_ADD(TestCoapCore::test_litecoap_send);
}
