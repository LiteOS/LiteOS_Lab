#ifndef __TEST_CASE_H
#define __TEST_CASE_H

#define TS_OK_HAS_DATA         1
#define TS_OK                  0
#define TS_FAILED             -1

#define TS_RECV_BUFFER_LEN 512


#define TEST_SORT_OC_LWM2M_AL  0
#define TEST_SORT_LWM2M_AL     1
#define TEST_SORT_OC_MQTT_AL   2
#define TEST_SORT_MQTT_AL      3
#define TEST_SORT_MAX          4

/*oc lwm2m*/
/*lwm2m al*/


/*oc mqtt*/
#define TEST_OC_MQTT_INIT             ((TEST_SORT_OC_MQTT_AL << 16) | 0)
#define TEST_OC_MQTT_REGISTER         ((TEST_SORT_OC_MQTT_AL << 16) | 1)
#define TEST_OC_MQTT_CONFIG           ((TEST_SORT_OC_MQTT_AL << 16) | 2)
#define TEST_OC_MQTT_JSON_FMT_REQ     ((TEST_SORT_OC_MQTT_AL << 16) | 3)
#define TEST_OC_MQTT_JSON_FMT_RES     ((TEST_SORT_OC_MQTT_AL << 16) | 4)
#define TEST_OC_MQTT_REPORT           ((TEST_SORT_OC_MQTT_AL << 16) | 5)
#define TEST_OC_MQTT_DECONFIG         ((TEST_SORT_OC_MQTT_AL << 16) | 6)
#define TEST_OC_MQTT_DEINIT           ((TEST_SORT_OC_MQTT_AL << 16) | 7)
#define TEST_OC_MQTT_GETVALUE         ((TEST_SORT_OC_MQTT_AL << 16) | 8)
#define TEST_OC_MQTT_INVALID          ((TEST_SORT_OC_MQTT_AL << 16) | 9)


/*mqtt al*/
#define TEST_MQTT_AL_INIT             ((TEST_SORT_MQTT_AL << 16) | 0)
#define TEST_MQTT_AL_DEINT            ((TEST_SORT_MQTT_AL << 16) | 1)
#define TEST_MQTT_AL_INSTALL          ((TEST_SORT_MQTT_AL << 16) | 2)
#define TEST_MQTT_AL_UNINSTALL        ((TEST_SORT_MQTT_AL << 16) | 3)
#define TEST_MQTT_AL_CONNECT          ((TEST_SORT_MQTT_AL << 16) | 4)
#define TEST_MQTT_AL_DISCONNECT       ((TEST_SORT_MQTT_AL << 16) | 5)
#define TEST_MQTT_AL_SUBSCRIBLE       ((TEST_SORT_MQTT_AL << 16) | 6)
#define TEST_MQTT_AL_UNSUBSCRIBLE     ((TEST_SORT_MQTT_AL << 16) | 7)
#define TEST_MQTT_AL_PUBLISH          ((TEST_SORT_MQTT_AL << 16) | 8)
#define TEST_MQTT_AL_CHECKSTATUS      ((TEST_SORT_MQTT_AL << 16) | 9)





typedef int (*test_sort)(int entry_id, char *message, int len);
typedef int (*test_entry)(char *message, int len);

void autotest_start(void);



int ts_sort_oc_lwm2m_al(int entry_id, char *message, int len);
int ts_sort_lwm2m_al(int entry_id, char *message, int len);
int ts_sort_oc_mqtt_al(int entry_id, char *message, int len);

int ts_sort_mqtt_al(int entry_id, char *message, int len);


#endif

