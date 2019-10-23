from com.huawei.iotplatform.client.dto.NBCommandResult import NBCommandResult


class NotifyNBCommandStatusChangedDTO(object):
    result = NBCommandResult()

    def __init__(self):
        self.deviceId = None
        self.commandId = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getCommandId(self):
        return self.commandId

    def setCommandId(self, commandId):
        self.commandId = commandId

    def getResult(self):
        return self.result

    def setResult(self, result):
        self.result = result
