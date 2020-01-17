import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DeviceGroupManagement import DeviceGroupManagement

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.CreateDeviceGroupInDTO import CreateDeviceGroupInDTO
from com.huawei.iotplatform.client.dto.CreateDeviceGroupOutDTO import CreateDeviceGroupOutDTO
from com.huawei.iotplatform.client.dto.DeviceGroupWithDeviceListDTO import DeviceGroupWithDeviceListDTO
from com.huawei.iotplatform.client.dto.ModifyDeviceGroupInDTO import ModifyDeviceGroupInDTO
from com.huawei.iotplatform.client.dto.QueryDeviceGroupMembersInDTO import QueryDeviceGroupMembersInDTO
from com.huawei.iotplatform.client.dto.QueryDeviceGroupsInDTO import QueryDeviceGroupsInDTO
from com.huawei.iotplatform.constant.Constant import Constant


class DeviceGroupManagementTest(object):
    def createDeviceGroupInfo(self):
        cdgInDTO = CreateDeviceGroupInDTO()
        cdgInDTO.name = "dcvwe"
        cdgInDTO.description = "hahahahaha_ge"
        cdgInDTO.deviceIds = ["s1", "s2"]
        cdgInDTO.id = "test01d2"
        return cdgInDTO

    def modifyDeviceGroupInfo(self):
        mdgInDTO = ModifyDeviceGroupInDTO()
        mdgInDTO.name = "zxccvasasd"
        return mdgInDTO

    def queryDeviceGroupMembersInfo(self):
        qdgmInDTO = QueryDeviceGroupMembersInDTO()
        qdgmInDTO.devGroupId = id
        return qdgmInDTO

    def addDevicesToGroupInfo(self):
        dgwdlDTO = DeviceGroupWithDeviceListDTO()
        dgwdlDTO.devGroupId = id
        dgwdlDTO.deviceIds = ["aa1", "bb1"]
        return dgwdlDTO

    def queryDeviceGroupsInfo(self):
        qdgInDTO = QueryDeviceGroupsInDTO()
        return qdgInDTO

    def deleteDevicesFromGroupInfo(self):
        dgwdlDTO = DeviceGroupWithDeviceListDTO()
        dgwdlDTO.devGroupId = id
        dgwdlDTO.deviceIds = [""]
        return dgwdlDTO


if __name__ == "__main__":
    dgmTest = DeviceGroupManagementTest()
    authentication = Authentication()
    deviceGroupManagement = DeviceGroupManagement()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # create a device group
    dc = deviceGroupManagement.createDeviceGroup(dgmTest.createDeviceGroupInfo(), accessToken)
    print("====== create a device group ======")
    print("result:", dc + "\n")

    # get devGroupId
    cdgOutDTO = CreateDeviceGroupOutDTO()
    cdgOutDTO.setId(json.loads(dc)['id'])
    id = cdgOutDTO.getId()
    print("id==", id + "\n")

    # modify a device group
    dm = deviceGroupManagement.modifyDeviceGroup(dgmTest.modifyDeviceGroupInfo(), id, None, accessToken)
    print("====== modify a device group ======")
    print("result:", dm + "\n")

    # query a specified device group
    dq = deviceGroupManagement.querySingleDeviceGroup(id, None, accessToken)
    print("====== query a specified device group ======")
    print("result:", dq + "\n")

    # query device group members
    dq = deviceGroupManagement.queryDeviceGroupMembers(dgmTest.queryDeviceGroupMembersInfo(), accessToken)
    print("====== query device group members ======")
    print("result:", dq + "\n")

    # add device group members
    da = deviceGroupManagement.addDevicesToGroup(dgmTest.addDevicesToGroupInfo(), None, accessToken)
    print("====== add device group members ======")
    print("result:", da + "\n")

    # delete device group members
    dd = deviceGroupManagement.deleteDeviceGroup(id, None, accessToken)
    print("====== delete device group members ======")
    print("result:", dd)
    print("--delete device group members succeeded--" + "\n")

    # query device groups
    dq = deviceGroupManagement.queryDeviceGroups(dgmTest.queryDeviceGroupsInfo(), accessToken)
    print("====== query device groups ======")
    print("result:", dq + "\n")

    # delete a device group
    dd = deviceGroupManagement.deleteDevicesFromGroup(dgmTest.deleteDevicesFromGroupInfo(), None, accessToken)
    print("====== delete a device group ======")
    print("result:", dd)
    print("--delete a device group succeeded--")
