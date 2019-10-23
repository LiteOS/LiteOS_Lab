class QueryDevicesInfoInDTO(object):

    def __init__(self):
        self.deviceId = None
        self.select = None
        self.appId = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getSelect(self):
        return self.select

    def setSelect(self, select):
        self.select = select

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId
