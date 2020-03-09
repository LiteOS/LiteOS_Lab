class DeviceDataHistoryDTO(object):

    def __init__(self):
        self.deviceId = None
        self.gatewayId = None
        self.appId = None
        self.serviceId = None
        self.data = None
        self.timestamp = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getGatewayId(self):
        return self.gatewayId

    def setGatewayId(self, gatewayId):
        self.gatewayId = gatewayId

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getData(self):
        return self.data

    def setData(self, data):
        self.data = data

    def getTimestamp(self):
        return self.timestamp

    def setTimestamp(self, timestamp):
        self.timestamp = timestamp
