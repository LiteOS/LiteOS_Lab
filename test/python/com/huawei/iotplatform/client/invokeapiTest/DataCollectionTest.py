import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DataCollection import DataCollection

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.QueryBatchDevicesInfoInDTO import QueryBatchDevicesInfoInDTO
from com.huawei.iotplatform.client.dto.QueryDeviceCapabilitiesInDTO import QueryDeviceCapabilitiesInDTO
from com.huawei.iotplatform.client.dto.QueryDeviceDataHistoryInDTO import QueryDeviceDataHistoryInDTO
from com.huawei.iotplatform.client.dto.QueryDeviceDesiredHistoryInDTO import QueryDeviceDesiredHistoryInDTO
from com.huawei.iotplatform.constant.Constant import Constant


class DataCollectionTest(object):
    def queryBatchDevicesInfo(self):
        qbdiInDTO = QueryBatchDevicesInfoInDTO()
        qbdiInDTO.gatewayId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qbdiInDTO.appId = "wk14orhpOumkAouraCpOsoQ3iO4a"
        return qbdiInDTO

    def queryDeviceDataHistory(self):
        qddhInDTO = QueryDeviceDataHistoryInDTO()
        qddhInDTO.deviceId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qddhInDTO.gatewayId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qddhInDTO.appId = "wk14orhpOumkAouraCpOsoQ3iO4a"
        return qddhInDTO

    def queryDeviceDesiredHistory(self):
        qddhInDTO = QueryDeviceDesiredHistoryInDTO()
        qddhInDTO.deviceId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qddhInDTO.gatewayId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qddhInDTO.appId = "wk14orhpOumkAouraCpOsoQ3iO4a"
        return qddhInDTO

    def queryDeviceCapabilities(self):
        qdcInDTO = QueryDeviceCapabilitiesInDTO()
        qdcInDTO.deviceId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qdcInDTO.gatewayId = "bd6394ed-b41d-430d-85f8-5d0290cfee24"
        qdcInDTO.appId = "wk14orhpOumkAouraCpOsoQ3iO4a"
        return qdcInDTO


if __name__ == "__main__":
    dcTest = DataCollectionTest()
    authentication = Authentication()
    dataCollection = DataCollection()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # query device info
    deviceId = "782eb286-9ead-424f-a910-c235b6090edc"
    dq = dataCollection.querySingleDeviceInfo(deviceId, None, None, accessToken)
    print("====== query device info ======")
    print("result:", dq + "\n")

    # query batch device info
    dataCollection.queryBatchDevicesInfo(dcTest.queryBatchDevicesInfo(), accessToken)
    print("====== query batch device info ======")
    print("result:", dq + "\n")

    # query device data history
    dq = dataCollection.queryDeviceDataHistory(dcTest.queryDeviceDataHistory(), accessToken)
    print("====== query device data history ======")
    print("result:", dq + "\n")

    # query device desired history
    dq = dataCollection.queryDeviceDesiredHistory(dcTest.queryDeviceDesiredHistory(), accessToken)
    print("====== query device desired history ======")
    print("result:", dq + "\n")

    # query device desired capabilities
    dq = dataCollection.queryDeviceCapabilities(dcTest.queryDeviceCapabilities(), accessToken)
    print("====== query device desired capabilities ======")
    print("result:", dq + "\n")
