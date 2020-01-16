class QueryDeviceCapabilitiesInDTO(object):

    def __init__(self):
        self.deviceId = None
        self.appId = None
        self.gatewayId = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getGatewayId(self):
        return self.gatewayId

    def setGatewayId(self, gatewayId):
        self.gatewayId = gatewayId
