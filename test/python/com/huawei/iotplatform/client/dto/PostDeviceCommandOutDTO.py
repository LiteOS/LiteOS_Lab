from com.huawei.iotplatform.client.dto.CommandDTOV4 import CommandDTOV4


class PostDeviceCommandOutDTO(object):

    def __init__(self):
        self.commandId = None
        self.appId = None
        self.deviceId = None
        self.command = CommandDTOV4()
        self.callbackUrl = None
        self.expireTime = None
        self.status = None
        self.result = None
        self.creationTime = None
        self.executeTime = None
        self.platformIssuedTime = None
        self.deliveredTime = None
        self.issuedTimes = None

    def getCommandId(self):
        return self.commandId

    def setCommandId(self, commandId):
        self.commandId = commandId

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getCommand(self):
        return self.command

    def setCommand(self, command):
        self.command = command

    def getCallbackUrl(self):
        return self.callbackUrl

    def setCallbackUrl(self, callbackUrl):
        self.callbackUrl = callbackUrl

    def getExpireTime(self):
        return self.expireTime

    def setExpireTime(self, expireTime):
        self.expireTime = expireTime

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getResult(self):
        return self.result

    def setResult(self, result):
        self.result = result

    def getCreationTime(self):
        return self.creationTime

    def setCreationTime(self, creationTime):
        self.creationTime = creationTime

    def getExecuteTime(self):
        return self.executeTime

    def setExecuteTime(self, executeTime):
        self.executeTime = executeTime

    def getPlatformIssuedTime(self):
        return self.platformIssuedTime

    def setPlatformIssuedTime(self, platformIssuedTime):
        self.platformIssuedTime = platformIssuedTime

    def getDeliveredTime(self):
        return self.deliveredTime

    def setDeliveredTime(self, deliveredTime):
        self.deliveredTime = deliveredTime

    def getIssuedTimes(self):
        return self.issuedTimes

    def setIssuedTimes(self, issuedTimes):
        self.issuedTimes = issuedTimes
