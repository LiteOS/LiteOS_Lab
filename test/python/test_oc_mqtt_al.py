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
    assert (dev_status == status)

def test_oc_mqtt_init():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_INIT, fname, mqtt_device_info.MQTT_EP_USER, mqtt_device_info.MQTT_EP_PASSWD, mqtt_device_info.MQTT_DEVICEID)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_INIT)
    assert (result.ret_code == 0)

def test_oc_mqtt_register():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_REGISTER, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_MQTT_REGISTER)
    assert(result.ret_code == 0)

def test_oc_mqtt_config():
    oc_mqtt_device_status_jduge(mqtt_device_info.MQTT_DEVICEID, "OFFLINE")
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_CONFIG, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_MQTT_CONFIG)
    assert(result.ret_code == 0)
    time.sleep(2)
    oc_mqtt_device_status_jduge(mqtt_device_info.MQTT_DEVICEID, "ONLINE")


def test_oc_mqtt_json_fmt_req():
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_JSON_FMT_REQ)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_JSON_FMT_REQ)
    assert (result.ret_code == 0)

def test_oc_mqtt_json_fmt_res():
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_JSON_FMT_RES)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_JSON_FMT_RES)
    assert (result.ret_code == 0)

def test_oc_mqtt_report():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_REPORT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_REPORT)
    assert (result.ret_code == 0)

def test_oc_mqtt_deconfig():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_DECONFIG, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_DECONFIG)
    assert (result.ret_code == 0)
    time.sleep(15)
    oc_mqtt_device_status_jduge(mqtt_device_info.MQTT_DEVICEID, "OFFLINE")

def test_oc_mqtt_deinit():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_MQTT_DEINIT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_MQTT_DEINIT)
    assert (result.ret_code == 0)
    resource_release()





if __name__ == '__main__':
    print("hello world")
    test_oc_mqtt_init()
    test_oc_mqtt_register()

