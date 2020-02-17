class CreateDeviceCmdCancelTaskInDTO(object):

    def __init__(self):
        self.deviceId = None
        self.appId = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId