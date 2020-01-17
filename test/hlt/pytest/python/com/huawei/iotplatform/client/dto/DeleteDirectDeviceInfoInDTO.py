class DeleteDirectDeviceInfoInDTO(object):

    def __init__(self):
        self.appId = None
        self.deviceId = None
        self.cascade = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getCascade(self):
        return self.cascade

    def setCascade(self, cascade):
        self.cascade = cascade
