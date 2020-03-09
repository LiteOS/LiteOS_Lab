#include "test_pcp.h"
#include <unistd.h>

#include <cstring>

extern "C"
{
	#include "ota_flag.h"
	#include "ota_adaptor.h"
	#include "string.h"
	#include "hal_flash.h"
	#include "osal.h"
	#include "pcp.h"
	#include "ota_flag.h"

	extern int osal_init(void);


	static uint8_t s_ota_flag[sizeof(ota_flag_t)];

	static int ota_flag_read(ota_flag_t *flag)
	{
    	memcpy(flag,s_ota_flag,sizeof(ota_flag_t));

    	return 0;
	}

	static int ota_flag_write(ota_flag_t *flag)
	{
    	memcpy(s_ota_flag,flag,sizeof(ota_flag_t));
    	return 0;
	}

	static int ota_bin_write(int offset ,void *msg,int len)
	{
    	printf("BIN W:%08d %08d\n\r",offset,len);
    	return 0;
	}

	static int ota_bin_read(int offset ,void *buf,int len)
	{
    	printf("BIN R:%08d %08d\n\r",offset,len);
    	return 0;

	}

	static ota_storage_t  s_ota_storage_device =
	{
      	name : "ota_storage",
		opt  : {
         	   		flag_read : ota_flag_read,
         			flag_write : ota_flag_write,
         			bin_write : ota_bin_write,
         			bin_read : ota_bin_read,
               }
	};
	static int ota_msg_send(void *msg,int len)
	{
    	int ret = 0;
    	return ret;
	}

}

TestPcp::TestPcp()
{
    TEST_ADD(TestPcp::test_ota_pcp_init);
    TEST_ADD(TestPcp::test_pcp_msg_push);
}

TestPcp::~TestPcp()
{

}

void TestPcp::test_ota_pcp_init(void)
{
	int ret;
	ret = ota_storage_uninstall(0);
	TEST_ASSERT(ret == 0);

	ret = ota_storage_install(&s_ota_storage_device);
	TEST_ASSERT(ret == 0);

	ret = ota_pcp_init(NULL);
    TEST_ASSERT(ret == -1);

	ret = ota_pcp_init(ota_msg_send);
    TEST_ASSERT(ret == 0);

}

void TestPcp::test_pcp_msg_push(void)
{
	int ret;
	uint8_t *msg = NULL;
	uint32_t msglen = 0;

	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	//起始标识0xFF,0xFE; 版本号0x01; 消息码 19; 校验码0x00,0x00; 数据区长度0x00,20; 数据区 16+分片2+分片数2+校验码2
	uint8_t PCP[40] = {0xFF, 0xFE, 0x01, 19, 0x00, 0x00, 0x00, 20, '0', '1', '2', '3', '4', '5', '6',\
									'7', '8', '9', '1', '2', '3', '4', '5', '6', 0x00, 32, 0x00, 0x02, 0x00, 0x12};


	////////////getversion
	msg = PCP;
	msglen = 40;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	//response_upgradestatus
	msg[3] = 24;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	////////////notifyblock
	msg[3] = 21;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	////////////notifyversion
	msg[3] = 20;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	msg[7] = 22;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);
	sleep(2);

	////////////notifyblock
	msg[3] = 21;
	msg[7] = 3;	msg[8] = 1;msg[9] = 1;msg[10] = 0;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	msg[7] = 22;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	msg[8] = 0;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	msg[9] = 0;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	msg[10] = 1;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);
	sleep(3);

	////////////response_downloadstatus
	msg[3] = 22;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	//default
	msg[3] = 25;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);

	//upgrade
	msg[3] = 23;
	ret = pcp_msg_push(msg, msglen);
	TEST_ASSERT(ret == 0);
	sleep(3);
}
