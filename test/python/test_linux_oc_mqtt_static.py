import pytest
import sys
import time
import json
import requests
from requests.adapters import HTTPAdapter
from requests import RequestException, ReadTimeout
from test_adapter import ts_call_single
from test_adapter import resource_release
from const import mqtt_device_info
from const import mqtt_testid
from com.huawei.iotplatform.constant.Constant import Constant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DeviceManagement import DeviceManagement
from com.huawei.iotplatform.client.invokeapiTest.DeviceManagementTest import DeviceManagementTest
from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.RegDirectDeviceOutDTO import RegDirectDeviceOutDTO


SERVER_IP4      = "49.4.93.24"
SERVER_PORT     = "8883"        #oc port
LIFE_TIME       = 10            #life time
CB_NAME         = "NULL"
# typedef enum
# {
#     en_oc_boot_strap_mode_factory = 0,
#     en_oc_boot_strap_mode_client_initialize,
# }en_oc_boot_strap_mode_t;
BOOTSTRAP_MODE  = 0

#typedef enum
#{
#    en_oc_mqtt_code_mode_binary = 0,   ///< the report and command is binary mode
#    en_oc_mqtt_code_mode_json,         ///< the report and command is json mode
#    en_oc_mqtt_code_mode_max,
#}en_oc_mqtt_code_mode;
CODE_MODE        = 1

#typedef enum
#{
#    en_mqtt_sign_type_hmacsha256_check_time_no = 0, //use HMACSHA256 to encode password but no check current time
#    en_mqtt_sign_type_hmacsha256_check_time_yes   , //use HMACSHA256 to encode password and check current time
#}en_oc_mqtt_sign_type;
SIGN_TYPE        = 0

#typedef enum
#{
#    en_mqtt_auth_type_devid = 0,
#    en_mqtt_auth_type_model = 1,
#    en_mqtt_auth_type_nodeid = 2,
#}en_oc_mqtt_auth_type;
AUTH_TYPE        = 2


# typedef enum
# {
#     en_oc_mqtt_device_type_dynamic = 0,
#     en_oc_mqtt_device_type_static,
# }en_oc_mqtt_device_type;
DEVICE_TYPE      = 1

# typedef enum
# {
# 	en_mqtt_al_security_none = 0,    ///< no encode
# 	en_mqtt_al_security_psk,         ///< use the psk mode in transport layer
# 	en_mqtt_al_security_cas,	     ///< use the ca mode in transport layer,only check the server
# 	en_mqtt_al_security_cacs,	     ///< use the ca mode in transport layer,both check the server and client
# 	en_mqtt_al_security_end,         ///< the end for the mqtt
# }en_mqtt_al_security_t;

SEC_TYPE         = 2
IS_CA_VALID      = 1



def oc_mqtt_device_status_jduge(deivceid, status):
    deviceManagement = DeviceManagement()
    # get accessToken at first
    result = Authentication().getAuthToken(Constant().clientInfo())
    print(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()
    # query device status
    dq = deviceManagement.queryDeviceStatus(deivceid, None, accessToken)
    print("====== query device status ======")
    print("result:", dq + "\n")
    assert (dq)
    assert (json.loads(dq)['activated'] == True)
    # query device shadow
    dq = deviceManagement.queryDeviceShadow(deivceid, None, accessToken)
    print("====== query device shadow ======")
    print("result:", dq + "\n")
    assert (dq)
    dev_status = json.loads(dq).get("deviceInfo").get("status")
    print("dev_status :", dev_status)
    assert (dev_status == status)

def test_linux_oc_mqtt_init_static():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_INIT, fname,
        mqtt_device_info.MQTT_EP_USER, mqtt_device_info.MQTT_EP_PASSWD,
        SERVER_IP4, SERVER_PORT, CB_NAME,
        BOOTSTRAP_MODE, LIFE_TIME, CODE_MODE,
        SIGN_TYPE, DEVICE_TYPE, AUTH_TYPE,
        SEC_TYPE, IS_CA_VALID)

    assert (result)
    print("id is ",result.test_id)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_INIT)
    assert (result.ret_code == 0)

def test_linux_oc_mqtt_register_static():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_REGISTER, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_MQTT_REGISTER)
    assert(result.ret_code == 0)

def test_linux_oc_mqtt_config_static():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_CONFIG, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_MQTT_CONFIG)
    assert(result.ret_code == 0)
    time.sleep(2)
    oc_mqtt_device_status_jduge(mqtt_device_info.MQTT_DEVICEID, "ONLINE")

def test_linux_oc_mqtt_json_fmt_req_static():
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


def test_linux_oc_mqtt_json_fmt_res_static():
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


def test_linux_oc_mqtt_report_static():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_REPORT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_REPORT)
    assert (result.ret_code == 0)

def test_linux_oc_mqtt_deconfig_static():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_DECONFIG, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_DECONFIG)
    assert (result.ret_code == 0)
    time.sleep(15)
    oc_mqtt_device_status_jduge(mqtt_device_info.MQTT_DEVICEID, "OFFLINE")

def test_linux_oc_mqtt_deinit_static():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_DEINIT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_DEINIT)
    assert (result.ret_code == 0)
    resource_release()



if __name__ == '__main__':
    print("hello world")
    test_linux_oc_mqtt_init_static()
    test_linux_oc_mqtt_config_static()
    test_linux_oc_mqtt_json_fmt_res_static()
    test_linux_oc_mqtt_deconfig_static()