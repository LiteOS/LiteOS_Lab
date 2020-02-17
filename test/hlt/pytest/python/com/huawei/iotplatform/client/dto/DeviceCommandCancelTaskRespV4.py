from com.huawei.iotplatform.client.dto.DeviceCommandRespV4 import DeviceCommandRespV4


class DeviceCommandCancelTaskRespV4(object):
    deviceCommands = DeviceCommandRespV4()

    def __init__(self):
        self.taskId = None
        self.appId = None
        self.deviceId = None
        self.status = None
        self.totalCount = None

    def getTaskId(self):
        return self.taskId

    def setTaskId(self, taskId):
        self.taskId = taskId

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getTotalCount(self):
        return self.totalCount

    def setTotalCount(self, totalCount):
        self.totalCount = totalCount

    def getDeviceCommands(self):
        return self.deviceCommands

    def setDeviceCommands(self, deviceCommands):
        self.deviceCommands = deviceCommands
