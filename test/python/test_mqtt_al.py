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


def test_mqtt_al_init():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_INIT, fname, mqtt_device_info.MQTT_EP_USER, mqtt_device_info.MQTT_EP_PASSWD, mqtt_device_info.MQTT_DEVICEID)
    print(result)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_MQTT_AL_INIT)
    assert (result.ret_code == 0)

def test_mqtt_al_install():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_INSTALL, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_MQTT_AL_INSTALL)
    assert (result.ret_code == 0)

def test_mqtt_al_uninstall():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_UNINSTALL, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_MQTT_AL_UNINSTALL)
    assert (result.ret_code == 0)

def test_mqtt_al_connect():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_CONNECT, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_MQTT_AL_CONNECT)
    assert(result.ret_code == 0)

def test_mqtt_al_disconnect():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_DISCONNECT, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_MQTT_AL_DISCONNECT)
    assert(result.ret_code == 0)

def test_mqtt_al_sub():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_SUBSCRIBLE, fname)
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_MQTT_AL_SUBSCRIBLE)
    assert(result.ret_code == 0)

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
    test_north_device()

