import json
import random

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DeviceManagement import DeviceManagement

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.ModifyDeviceInfoInDTO import ModifyDeviceInfoInDTO
from com.huawei.iotplatform.client.dto.ModifyDeviceShadowInDTO import ModifyDeviceShadowInDTO
from com.huawei.iotplatform.client.dto.QueryDeviceRealtimeLocationInDTO import QueryDeviceRealtimeLocationInDTO
from com.huawei.iotplatform.client.dto.RefreshDeviceKeyInDTO import RefreshDeviceKeyInDTO
from com.huawei.iotplatform.client.dto.RegDirectDeviceInDTO import RegDirectDeviceInDTO
from com.huawei.iotplatform.client.dto.RegDirectDeviceOutDTO import RegDirectDeviceOutDTO
from com.huawei.iotplatform.constant.Constant import Constant


class DeviceManagementTest(object):
    def regDirectDeviceInfo(self):
        rddInDto = RegDirectDeviceInDTO()
        rddInDto.nodeId = "AAA" + str(random.randint(0, 9999))
        return rddInDto

    def modifyDeviceInfo(self):
        mdiInDto = ModifyDeviceInfoInDTO()
        mdiInDto.deviceId = "180909a3-1b16-46bb-9f9f-d737c201ca81"
        mdiInDto.manufacturerId = "eSDK04231"
        mdiInDto.mute = "TRUE"
        mdiInDto.isSecure = "true"
        return mdiInDto

    def queryDeviceRealtimeLocationInfo(self):
        qdrlInDTO = QueryDeviceRealtimeLocationInDTO()
        qdrlInDTO.horAcc = 1000
        qdrlInDTO.geoInfo.identifier.edition = "6.1"
        qdrlInDTO.geoInfo.identifier.codeSpace = "EPSG"
        qdrlInDTO.geoInfo.identifier.code = "4326"
        return qdrlInDTO

    def modifyDeviceShadowInfo(self):
        mdsInDTO = ModifyDeviceShadowInDTO()
        mdsInDTO.serviceDesireds = [
            {mdsInDTO.serviceDesireds.serviceId: "Discovery", mdsInDTO.serviceDesireds.desired: {}}]
        return mdsInDTO

    def refreshDeviceKeyInfo(self):
        rdkInDTO = RefreshDeviceKeyInDTO()
        rdkInDTO.deviceId = deviceId
        rdkInDTO.nodeId = "SSSDAWSQAS"
        rdkInDTO.verifyCode = "SSSDAWSQAS11"
        return rdkInDTO


if __name__ == "__main__":
    dmTest = DeviceManagementTest()
    authentication = Authentication()
    deviceManagement = DeviceManagement()

    # get accessToken at first
    result = Authentication().getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # register a new device
    dr = deviceManagement.regDirectDevice(dmTest.regDirectDeviceInfo(), None, accessToken)
    print("====== register a new device ======")
    print("result:", dr + "\n")

    # get deviceId
    rddod = RegDirectDeviceOutDTO()
    rddod.setDeviceId(json.loads(dr)['deviceId'])
    deviceId = rddod.getDeviceId()
    print("deviceId==", deviceId + "\n")

    # modify device info
    dm = deviceManagement.modifyDeviceInfo(dmTest.modifyDeviceInfo(), deviceId, None, accessToken)
    print("====== modify device info ======")
    print("result:", dm + "\n")

    # query device status
    dq = deviceManagement.queryDeviceStatus(deviceId, None, accessToken)
    print("====== query device status ======")
    print("result:", dq + "\n")

    # query device real-time location 
    dq = deviceManagement.queryDeviceRealtimeLocation(dmTest.queryDeviceRealtimeLocationInfo(), None, accessToken)
    print("====== query device real-time location ======")
    print("result:", dq + "\n")

    # modify device shadow
    dm = deviceManagement.modifyDeviceShadow(dmTest.modifyDeviceShadowInfo(), deviceId, None, accessToken)
    print("====== modify device shadow ======")
    print("result:", dm + "\n")

    # query device shadow
    dq = deviceManagement.queryDeviceShadow(deviceId, None, accessToken)
    print("====== query device shadow ======")
    print("result:", dq + "\n")

    # refresh device key
    dr = deviceManagement.refreshDeviceKey(dmTest.refreshDeviceKeyInfo(), None, accessToken)
    print("====== refresh device key ======")
    print("result:", dr + "\n")

    # delete device
    dd = deviceManagement.deleteDirectDevice(deviceId, "true", None, accessToken)
    print("====== delete device ======")
    print("result:", dd)
    print("- delete device succeeded -")
