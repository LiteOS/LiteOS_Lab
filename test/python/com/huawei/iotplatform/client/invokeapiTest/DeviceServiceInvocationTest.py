import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DeviceServiceInvocation import DeviceServiceInvocation

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.CommandDTO import CommandDTO
from com.huawei.iotplatform.constant.Constant import Constant


class DeviceServiceInvocationTest(object):
    def invokeDeviceServiceInfo(self):
        commandDTO = CommandDTO()
        commandDTO.header = {
            commandDTO.header.mode: "ACK",
            commandDTO.header.method: "DISCOVERY",
        }
        return commandDTO


if __name__ == "__main__":
    dsiTest = DeviceServiceInvocationTest()
    authentication = Authentication()
    DeviceServiceInvocation = DeviceServiceInvocation()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()
    deviceId = "36f97413-814b-4cb7-b999-f527e0a74a6d"
    serviceId = "Discovery"

    # invoke device service
    di = DeviceServiceInvocation.invokeDeviceService(dsiTest.invokeDeviceServiceInfo(), deviceId, serviceId, None,
                                                     accessToken)
    print("====== invoke device service ======")
    print("result:", di)
