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
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_INIT, fname,  "192.168.1.103", "8883", "test", "test123", "YES", "CAVALID")
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
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_SUBSCRIBLE, fname, "test")
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_MQTT_AL_SUBSCRIBLE)
    assert(result.ret_code == 0)


def test_mqtt_al_unsub():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_UNSUBSCRIBLE, fname, "test")
    assert(result)
    assert(result.test_id == mqtt_testid.TEST_MQTT_AL_UNSUBSCRIBLE)
    assert(result.ret_code == 0)

def test_mqtt_al_pub():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_PBULISH, fname, "test22", "hello world")
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_MQTT_AL_PBULISH)
    assert (result.ret_code == 0)

def test_mqtt_al_checkstatus():
    fname = sys._getframe().f_code.co_name
    result = ts_call_single(mqtt_testid.TEST_MQTT_AL_CHECKSTATUS, fname)
    assert (result)
    assert (result.test_id == mqtt_testid.TEST_MQTT_AL_CHECKSTATUS)
    assert (result.ret_code == 0)

if __name__ == '__main__':
    print("hello world")
    test_mqtt_al_init()
    test_mqtt_al_install()
    test_mqtt_al_connect()
    test_mqtt_al_sub()
    test_mqtt_al_unsub()
    test_mqtt_al_pub()
    test_mqtt_al_checkstatus()
    test_mqtt_al_disconnect()
    test_mqtt_al_uninstall()


