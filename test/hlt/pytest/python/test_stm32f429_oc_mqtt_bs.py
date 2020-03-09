import pytest
import sys
import time
import json
import requests
from requests.adapters import HTTPAdapter
from requests import RequestException, ReadTimeout
from test_adapter import ts_call_single
from test_adapter import resource_release
from test_adapter import setDestHost
from const import mqtt_device_info
from const import mqtt_testid
from iotlink_north_invoke import iotlink_north_invoke

BS_SERVER_IP4       = "119.3.251.30"
BS_SERVER_PORT      = "8883"
SERVER_IP4          = "49.4.93.24"
SERVER_PORT         = "8883"        #oc port
INVALID_SERVER_IP4  = "NULL"
INVALID_SERVER_PORT = "NULL"
LIFE_TIME           = 10            #life time
CB_NAME             = "YES"#"NULL"
# typedef enum
# {
#     en_oc_mqtt_mode_bs_static_nodeid_hmacsha256_notimecheck_json =0,
#     en_oc_mqtt_mode_nobs_static_nodeid_hmacsha256_notimecheck_json,
#     en_oc_mqtt_mode_last,
# }en_oc_mqtt_mode;
en_oc_boot_strap_mode_factory             = 1
en_oc_boot_strap_mode_client_initialize   = 0
BOOTSTRAP_MODE  = en_oc_boot_strap_mode_factory


# typedef enum
# {
# 	en_mqtt_al_security_none = 0,    ///< no encode
# 	en_mqtt_al_security_psk,         ///< use the psk mode in transport layer
# 	en_mqtt_al_security_cas,	     ///< use the ca mode in transport layer,only check the server
# 	en_mqtt_al_security_cacs,	     ///< use the ca mode in transport layer,both check the server and client
# 	en_mqtt_al_security_end,         ///< the end for the mqtt
# }en_mqtt_al_security_t;

SEC_TYPE         = 2



def test_stm32f429_oc_mqtt_init():
    setDestHost('192.168.1.115')

    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_INIT, fname)
    assert (result)
    print("id is ",result.test_id)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_INIT)
    assert (result.ret_code == 0)

def test_stm32f429_oc_mqtt_register():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_REGISTER, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_MQTT_REGISTER)
    assert(result.ret_code == 0)

def test_stm32f429_oc_mqtt_config_bs():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_CONFIG, fname,
                            en_oc_boot_strap_mode_client_initialize, LIFE_TIME,
                            BS_SERVER_IP4, BS_SERVER_PORT, SEC_TYPE,
                            mqtt_device_info.BS_MQTT_EP_USER, mqtt_device_info.BS_MQTT_EP_PASSWD,
                            CB_NAME)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_MQTT_CONFIG)
    assert(result.ret_code == 0)
    time.sleep(2)
    # oc_mqtt_device_status_jduge_b(mqtt_device_info.BS_MQTT_DEVICEID, "ONLINE")

def test_stm32f429_oc_mqtt_json_fmt_req_bs():
    fname = sys._getframe().f_code.co_name
    #must send 4 items here
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_JSON_FMT_REQ, fname,
                            "LED", 0,    #LED is serviceId,0 means en_oc_mqtt_has_more_no
                            "LED1", 0, 1,#LED1 is item name, 0 means en_key_value_type_int, 1 means value
                            "LED2", 0, 2,
                            "LED3", 0, 35,
                            "LED4", 0, 78
                            )
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_JSON_FMT_REQ)
    assert (result.ret_code == 1)
    print("----------------------------------------------")
    print("hhhh")
    print(json.loads(result.data).get("data")[0])
    print("----------------------------------------------")
    # typedef enum
    # {
    #     en_oc_mqtt_has_more_no = 0,
    #     en_oc_mqtt_has_more_yes = 1,
    # }en_oc_mqtt_has_more;
    # typedef enum
    # {
    #     en_key_value_type_int = 0,
    #     en_key_value_type_string,
    #     en_key_value_type_array,
    # }en_value_type;
    # parase received json data
    # {
    #     "msgType": "deviceReq",
    #     "hasMore": 0,
    #     "data": [{
    #         "serviceId": "LED",
    #         "serviceData": {
    #             "LED1": 1,
    #             "LED2": 2,
    #             "LED2": 35,
    #             "LED2": 78
    #         }
    #     }]
    # }

    assert (json.loads(result.data).get("msgType") == "deviceReq")
    assert (json.loads(result.data).get("hasMore") == 0)
    data = json.loads(result.data).get("data")[0]
    assert (data["serviceId"] == "LED")
    print("+++++++++++++++++++++++++++++++++++++++++++")
    print(result.data)
    print(data)
    print(data["serviceData"])
    print("+++++++++++++++++++++++++++++++++++++++++++")
    assert (data.get("serviceData").get("LED1") == 1)
    assert (data.get("serviceData").get("LED2") == 2)
    assert (data.get("serviceData").get("LED3") == 35)
    assert (data.get("serviceData").get("LED4") == 78)


def test_stm32f429_oc_mqtt_json_fmt_res_bs():
    fname = sys._getframe().f_code.co_name
    # must send 1 items here
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_JSON_FMT_RES, fname,
                            0, 0, 1, # 0 means en_oc_mqtt_has_more_no, second means no error, 1 means mid
                            "body_para", 1, "body_para"  # body_para is item name, second means string type, third means value
                            )
    print(result)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_JSON_FMT_RES)
    assert (result.ret_code == 1)
    print(result.data)

    # {
    #     "msgType": "deviceRsp",
    #     "mid": 1,
    #     "errcode": 0,
    #     "hasMore": 0,
    #     "body": {
    #         "body_para": "body_para"
    #     }
    # }

    assert (json.loads(result.data).get("msgType") == "deviceRsp")
    assert (json.loads(result.data).get("mid") == 1)
    assert (json.loads(result.data).get("errcode") == 0)
    assert (json.loads(result.data).get("hasMore") == 0)
    assert (json.loads(result.data).get("body").get("body_para") == "body_para")


def test_stm32f429_oc_mqtt_publish_bs():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_PUBLISH, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_PUBLISH)
    assert (result.ret_code == 0)

def test_stm32f429_oc_mqtt_deconfig_bs():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_DECONFIG, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_DECONFIG)
    assert (result.ret_code == 0)
    time.sleep(15)
    # oc_mqtt_device_status_jduge_b(mqtt_device_info.BS_MQTT_DEVICEID, "OFFLINE")



def test_stm32f429_oc_mqtt_deinit():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_DEINIT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_DEINIT)
    assert (result.ret_code == 0)
    resource_release()



if __name__ == '__main__':
    print("hello world")
    test_stm32f429_oc_mqtt_init()
    test_stm32f429_oc_mqtt_register()
    # test_stm32f429_oc_mqtt_config_bs()
    # test_stm32f429_oc_mqtt_json_fmt_req_bs()
    # test_stm32f429_oc_mqtt_json_fmt_res_bs()
    # test_stm32f429_oc_mqtt_report_bs()
    # test_stm32f429_oc_mqtt_deconfig_bs()
    test_stm32f429_oc_mqtt_deinit()
