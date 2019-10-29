import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.DeviceUpgrade import DeviceUpgrade

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.CreateUpgradeTaskInDTO import CreateUpgradeTaskInDTO
from com.huawei.iotplatform.client.dto.OperateDevices import OperateDevices
from com.huawei.iotplatform.client.dto.QueryUpgradePackageListInDTO import QueryUpgradePackageListInDTO
from com.huawei.iotplatform.client.dto.QueryUpgradeSubTaskInDTO import QueryUpgradeSubTaskInDTO
from com.huawei.iotplatform.client.dto.QueryUpgradeTaskListInDTO import QueryUpgradeTaskListInDTO
from com.huawei.iotplatform.constant.Constant import Constant


class DeviceUpgradeTest(object):
    def queryUpgradePackageListInfo(self):
        quplInDTO = QueryUpgradePackageListInDTO()
        quplInDTO.fileType = "firmwarePackage"
        return quplInDTO

    def createFirmwareUpgradeTaskInfo(self):
        cutInDTO = CreateUpgradeTaskInDTO()
        od = OperateDevices()
        cutInDTO.fileId = "ss"
        cutInDTO.targets = {od.devices: ['sss']}
        return cutInDTO

    def queryUpgradeSubTaskInfo(self):
        qustInDTO = QueryUpgradeSubTaskInDTO()
        qustInDTO.pageSize = "1"
        return qustInDTO

    def queryUpgradeTaskListInfo(self):
        qutlInDTO = QueryUpgradeTaskListInDTO()
        qutlInDTO.deviceId = "80361c26-f24f-4c6b-a4e2-acbf31bc3282"
        return qutlInDTO

    def createSoftwareUpgradeTaskInfo(self):
        cutInDTO = CreateUpgradeTaskInDTO()
        od = OperateDevices()
        cutInDTO.fileId = "ss"
        cutInDTO.targets = {od.devices: ['sss']}
        return cutInDTO


if __name__ == "__main__":
    duTest = DeviceUpgradeTest()
    authentication = Authentication()
    deviceUpgrade = DeviceUpgrade()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()
    fileId = "aaaa"
    operationId = "aaa"

    # query upgrade package list
    dq = deviceUpgrade.queryUpgradePackageList(duTest.queryUpgradePackageListInfo(), accessToken)
    print("====== query upgrade package list (firmware and software) ======")
    print("result:", dq + "\n")

    # query a specified upgrade package
    dq = deviceUpgrade.queryUpgradePackage(fileId, accessToken)
    print("====== query a specified upgrade package ======")
    print("result:", dq + "\n")

    # create a firmware upgrade task
    dc = deviceUpgrade.createFirmwareUpgradeTask(duTest.createFirmwareUpgradeTaskInfo(), accessToken)
    print("====== create a firmware upgrade task ======")
    print("result:", dc + "\n")

    # query the upgrade task
    dq = deviceUpgrade.queryUpgradeTask(operationId, accessToken)
    print("====== query the upgrade task ======")
    print("result:", dq + "\n")

    # query the upgrade task detail
    dq = deviceUpgrade.queryUpgradeSubTask(duTest.queryUpgradeSubTaskInfo(), operationId, accessToken)
    print("====== query the upgrade task detail ======")
    print("result:", dq + "\n")

    # delete a specified upgrade package
    dd = deviceUpgrade.deleteUpgradePackage(fileId, accessToken)
    print("====== delete a specified upgrade package ======")
    print("result:", dd)
    print("--delete a specified upgrade package succeeded--" + "\n")

    # query upgrade task list
    dq = deviceUpgrade.queryUpgradeTaskList(duTest.queryUpgradeTaskListInfo(), accessToken)
    print("====== query upgrade task list ======")
    print("result:", dq + "\n")

    # create a software upgrade task
    dc = deviceUpgrade.createSoftwareUpgradeTask(duTest.createSoftwareUpgradeTaskInfo(), accessToken)
    print("====== create a software upgrade task ======")
    print("result:", dc + "\n")
