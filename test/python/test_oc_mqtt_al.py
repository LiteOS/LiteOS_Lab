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


def test_oc_mqtt_init():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_ATINY_MQTT_INIT, fname, mqtt_device_info.MQTT_EP_USER, mqtt_device_info.MQTT_EP_PASSWD, mqtt_device_info.MQTT_DEVICEID)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_ATINY_MQTT_INIT)
    assert (result.ret_code == 0)

def test_oc_mqtt_connect():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_ATINY_MQTT_CONNECT, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_OC_ATINY_MQTT_CONNECT)
    assert(result.ret_code == 0)

# def test_oc_mqtt_subcmd():
#     result = ts_call_single(mqtt_testid.TEST_OC_ATINY_MQTT_SUBCMD)
#     assert (result)
#     assert (result.test_id == mqtt_testid.TEST_OC_MQTT_SUBCMD)
#     assert (result.ret_code == 0)
#
def test_oc_mqtt_report():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_ATINY_MQTT_REPORT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_ATINY_MQTT_REPORT)
    assert (result.ret_code == 0)

def test_oc_device_status_query():
    # 实例化
    print("call test_oc_device_status_query")
    deviceManagement = DeviceManagement()
    # get accessToken at first
    result = Authentication().getAuthToken(Constant().clientInfo())
    print(result)
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()
    # status = deviceManagement.queryDeviceStatus(mqtt_device_info.MQTT_DEVICEID, Constant().clientInfo()["appId"], accessToken)
    status = deviceManagement.queryDeviceStatus(mqtt_device_info.MQTT_DEVICEID, None, accessToken)
    print(status)
    assert(status)
    assert(json.loads(status)['activated'] == True)
#
def test_oc_mqtt_disconnect():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_ATINY_MQTT_DISCONNECT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_ATINY_MQTT_DISCONNECT)
    assert (result.ret_code == 0)

def test_oc_mqtt_deinit():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_OC_ATINY_MQTT_DEINIT, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_OC_ATINY_MQTT_DEINIT)
    assert (result.ret_code == 0)
    resource_release()

def test_north_device():

    deviceManagement = DeviceManagement()
    deviceId = "64e20563-4939-4c44-924f-b439b8dae1fd"
    # get accessToken at first
    result = Authentication().getAuthToken(Constant().clientInfo())
    print(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # register a new device

    # query device status
    dq = deviceManagement.queryDeviceStatus(deviceId, None, accessToken)
    print("====== query device status ======")
    print("result:", dq + "\n")

    # query device shadow
    dq = deviceManagement.queryDeviceShadow(deviceId, None, accessToken)
    print("====== query device shadow ======")
    print("result:", dq + "\n")



if __name__ == '__main__':
    print("hello world")
    #test_north_device()

