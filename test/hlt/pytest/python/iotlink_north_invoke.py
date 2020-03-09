import sys
import time
import json
from com.huawei.iotplatform.constant.Constant import Constant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DeviceManagement import DeviceManagement
from com.huawei.iotplatform.client.invokeapiTest.DeviceManagementTest import DeviceManagementTest
from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.RegDirectDeviceOutDTO import RegDirectDeviceOutDTO
from com.huawei.iotplatform.client.dto.ClientInfo import ClientInfo
from com.huawei.iotplatform.client.invokeapi.DeviceServiceInvocation import DeviceServiceInvocation
from com.huawei.iotplatform.client.dto.CommandDTO import CommandDTO
from com.huawei.iotplatform.client.invokeapi.SignalDelivery import SignalDelivery
from com.huawei.iotplatform.client.dto.PostDeviceCommandInDTO import PostDeviceCommandInDTO
from com.huawei.iotplatform.client.dto.PostDeviceCommandOutDTO import PostDeviceCommandOutDTO
from com.huawei.iotplatform.client.dto.CommandDTOV4 import CommandDTOV4

class iotlink_north_invoke(object):
    def __init__(self):
        self.developClient = Constant().clientInfoEx("CLIENT_INFO")
        self.businessClient = Constant().clientInfoEx("CLIENTBUSINESS_INFO")
        print(self.developClient)
        print(self.businessClient)

    def oc_mqtt_get_device_shadowstatus(self, deivceid):
        deviceManagement = DeviceManagement()
        # get accessToken at first
        result = Authentication().getAuthToken(self.developClient)
        print(result)
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()
        # query device status
        dq = deviceManagement.queryDeviceStatus(deivceid, None, accessToken)
        print("====== query device status ======")
        print("result:", dq + "\n")
        # query device shadow
        dq = deviceManagement.queryDeviceShadow(deivceid, None, accessToken)
        print("====== query device shadow ======")
        print("result:", dq + "\n")
        dev_status = json.loads(dq).get("deviceInfo").get("status")
        print("dev_status :", dev_status)
        return dev_status

    def oc_mqtt_get_device_shadowstatus_business(self, deivceid):
        deviceManagement = DeviceManagement()
        result = Authentication().getAuthToken(self.businessClient)
        print(result)
        print(json.loads(result))
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()
        print(accessToken)
        # query device shadow
        dq = deviceManagement.queryDeviceShadow(deivceid, None, accessToken)
        print("====== query device shadow ======")
        print("result:", dq + "\n")

        dev_status = json.loads(dq).get("deviceInfo").get("status")
        print("dev_status :", dev_status)
        return dev_status

    def oc_mqtt_device_cmd_send(self, deviceId, serviceId, mode, method, payload):
        # get accessToken at first
        appId = self.developClient["appId"]

        dss = DeviceServiceInvocation()
        result = Authentication().getAuthToken(Constant().clientInfo())
        print(Constant().clientInfo())
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()
        # invoke device service
        commandDTO = CommandDTO()
        commandDTO.header = {
            commandDTO.header.mode: mode,
            commandDTO.header.method: method
        }

        commandDTO.body = payload

        di = dss.invokeDeviceService(commandDTO, deviceId, serviceId, appId,
                                     accessToken)
        print("====== invoke device service ======")
        print("result:", di)
     
    def oc_lwm2m_get_device_shadowstatus(self, deivceid):
        deviceManagement = DeviceManagement()
        # get accessToken at first
        result = Authentication().getAuthToken(self.developClient)
        print(result)
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()
        # query device status
        dq = deviceManagement.queryDeviceStatus(deivceid, None, accessToken)
        print("====== query device status ======")
        print("result:", dq + "\n")
        # query device shadow
        dq = deviceManagement.queryDeviceShadow(deivceid, None, accessToken)
        print("====== query device shadow ======")
        print("result:", dq + "\n")
        dev_status = json.loads(dq).get("deviceInfo").get("status")
        print("dev_status :", dev_status)
        return dev_status


    def oc_lwm2m_device_cmd_send(self, deviceId, serviceId, mode, method, payload):
        # get accessToken at first   
        result = Authentication().getAuthToken(self.developClient)
        print("AuthToken: ", result)
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()       
        # appId
        appId = self.developClient["appId"] 
        # invoke device service
        pdcInDTO = PostDeviceCommandInDTO()
        pdcInDTO.deviceId = deviceId
        pdcInDTO.command = {
            pdcInDTO.command.method: method,
            pdcInDTO.command.serviceId: serviceId,
            pdcInDTO.command.paras: payload 
        }
        # send command to device
        cmd_class = SignalDelivery
        cmd_result = cmd_class.postDeviceCommand(self, pdcInDTO, appId, accessToken)      
        print("====== invoke device service ======")
        print("result:", cmd_result)  
       
        # get commandId And deviceId
        pdcOutDTO = PostDeviceCommandOutDTO()
        pdcOutDTO.setCommandId(json.loads(cmd_result)['commandId'])
        commandId = pdcOutDTO.getCommandId()
        pdcOutDTO.setDeviceId(json.loads(cmd_result)['deviceId'])
        deviceId = pdcOutDTO.getDeviceId()
        print("commandId==", commandId)
        print("deviceId==", deviceId + "\n")
    
    def oc_coap_get_device_shadowstatus(self, deivceid):
        deviceManagement = DeviceManagement()
        # get accessToken at first
        result = Authentication().getAuthToken(self.developClient)
        print(result)
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()
        # query device status
        dq = deviceManagement.queryDeviceStatus(deivceid, None, accessToken)
        print("====== query device status ======")
        print("result:", dq + "\n")
        # query device shadow
        dq = deviceManagement.queryDeviceShadow(deivceid, None, accessToken)
        print("====== query device shadow ======")
        print("result:", dq + "\n")
        dev_status = json.loads(dq).get("deviceInfo").get("status")
        print("dev_status :", dev_status)
        return dev_status


    def oc_coap_device_cmd_send(self, deviceId, serviceId, method, payload):
        appId = self.developClient["appId"]
        dss = SignalDelivery()
        result = Authentication().getAuthToken(Constant().clientInfo())
        print(Constant().clientInfo())
        authOutDTO = AuthOutDTO()
        authOutDTO.setAccessToken(json.loads(result)['accessToken'])
        accessToken = authOutDTO.getAccessToken()
        pdcInDTO = PostDeviceCommandInDTO()
        pdcInDTO.deviceId = deviceId
        pdcInDTO.command = {
            pdcInDTO.command.method: method,
            pdcInDTO.command.serviceId: serviceId,
            pdcInDTO.command.paras: payload
            }
        di = dss.postDeviceCommand(pdcInDTO, appId, accessToken)
        print("====== invoke device service ======")
        print("result:", di)	
	

