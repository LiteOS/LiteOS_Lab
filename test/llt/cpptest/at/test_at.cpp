#include "test_at.h"
#include <cstdio>
#include <cstring>
#include "shell.h"

extern "C" {
#include "at.h"
#include <unistd.h>
int osal_init(void);
extern int at_oobregister(const char *name,const void *index,size_t len,fn_at_oob func,void *args);
extern int at_streammode_set(int mode);
extern struct shell_item_t __oshell_shell_at;
extern struct shell_item_t __oshell_shell_atdebug;

    bool_t uart_at_init(int baud)
    {
        return true;
    }
}

using namespace std;

TEST_at::TEST_at()
{
    osal_init();

    TEST_ADD(TEST_at::test_at_init);
    TEST_ADD(TEST_at::test_at_oobregister);
    TEST_ADD(TEST_at::test_at_command);
    TEST_ADD(TEST_at::test_at_streammode_set);
    TEST_ADD(TEST_at::test_shell_at);
    TEST_ADD(TEST_at::test_shell_atdebug);
}

void TEST_at::test_at_init()
{
    int ret;
    ret = at_init(NULL);
    TEST_ASSERT (ret == -1);

    ret = at_init("test_at_device");
    TEST_ASSERT (ret == 0);
}

static int test_device_callbak(void *args, void *data, size_t datalen)
{
    cout << "I`m callbak" << endl;
    return 0;
}

void TEST_at::test_at_oobregister()
{
    int ret;

    ret = at_oobregister("test_device", NULL, strlen("TEST+"), test_device_callbak, NULL);
    TEST_ASSERT (ret == -1);

    ret = at_oobregister("test_device", "TEST+", strlen("TEST+"), NULL, NULL);
    TEST_ASSERT (ret == -1);

    ret = at_oobregister("test_device", "TEST+", strlen("TEST+"), test_device_callbak, NULL);
    TEST_ASSERT (ret == 0);
}

void TEST_at::test_at_streammode_set()
{
    (void)at_streammode_set(1);
}

void TEST_at::test_at_command()
{
    int ret;
    ret = at_command(NULL, 0, "AT_test", NULL, 0, 1000);
    TEST_ASSERT (ret == -1);

    ret = at_command("ok", strlen("ok"), NULL, NULL, 0, 1000);
    TEST_ASSERT (ret == 0);

    ret = at_command("TEST+help", strlen("TEST+help"), "OK", NULL, 0, 1000);
    TEST_ASSERT (ret == strlen("TEST+help"));

    (void)at_streammode_set(1);
    ret = at_command("TEST+help", strlen("TEST+help"), "OK", NULL, 0, 1000);
    TEST_ASSERT (ret >= (int)strlen("TEST+help"));

    ret = at_command("TEST+help", strlen("TEST+help"), "ERROR", NULL, 0, 1000);
    TEST_ASSERT (ret == -1);

    (void)at_streammode_set(0);
    ret = at_command("TEST+help", strlen("TEST+help"), "OK", NULL, 0, 1000);
    TEST_ASSERT (ret == strlen("TEST+help"));

    ret = at_command("TEST+help", strlen("TEST+help"), "ERROR", NULL, 0, 1000);
    TEST_ASSERT (ret == -1);
}

void TEST_at::test_shell_atdebug()
{
	fn_shell_cmdentry atdebug_entry = (fn_shell_cmdentry) __oshell_shell_atdebug.addr;
	const char *argv1[] = {"atdebug"};
	const char *argv2[] = {"atdebug", "rx", "none"};
	const char *argv3[] = {"atdebug", "rx", "ascii"};
	const char *argv4[] = {"atdebug", "rx", "hex"};
	const char *argv5[] = {"atdebug", "rx", "octal"};
	const char *argv6[] = {"atdebug", "tx", "none"};
	const char *argv7[] = {"atdebug", "trx", "none"};
	const char *argv8[] = {"atdebug", "tx", "hex"};
    int ret;

    ret = atdebug_entry(sizeof(argv1)/sizeof(char *), argv1);
    TEST_ASSERT (ret == -1);

    ret = atdebug_entry(sizeof(argv2)/sizeof(char *), argv2);
    TEST_ASSERT (ret == 0);

    ret = atdebug_entry(sizeof(argv3)/sizeof(char *), argv3);
    TEST_ASSERT (ret == 0);

    ret = atdebug_entry(sizeof(argv5)/sizeof(char *), argv5);
    TEST_ASSERT (ret == -2);

    ret = atdebug_entry(sizeof(argv6)/sizeof(char *), argv6);
    TEST_ASSERT (ret == 0);

    ret = atdebug_entry(sizeof(argv7)/sizeof(char *), argv7);
    TEST_ASSERT (ret == -3);

    ret = atdebug_entry(sizeof(argv4)/sizeof(char *), argv4);
    TEST_ASSERT (ret == 0);

    ret = atdebug_entry(sizeof(argv8)/sizeof(char *), argv8);
    TEST_ASSERT (ret == 0);

	(void)at_command("TEST+help", strlen("TEST+help"), "OK", NULL, 0, 1000);
}

void TEST_at::test_shell_at()
{
	fn_shell_cmdentry at_command_entry = (fn_shell_cmdentry) __oshell_shell_at.addr;
	const char *argv1[] = {"atcmd"};
	const char *argv3[] = {"atcmd", "ok"};
	const char *argv4[] = {"atcmd", "TEST+help", "OK"};
    int ret;

    ret = at_command_entry(sizeof(argv1)/sizeof(char *), argv1);
    TEST_ASSERT (ret == -1);

    ret = at_command_entry(sizeof(argv3)/sizeof(char *), argv3);
    TEST_ASSERT (ret == 0);

    ret = at_command_entry(sizeof(argv4)/sizeof(char *), argv4);
    TEST_ASSERT (ret == strlen("TEST+help"));
}
