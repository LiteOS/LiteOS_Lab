class DeviceDesiredHistoryDTO(object):

    def __init__(self):
        self.serviceId = None
        self.deviceId = None
        self.gatewayId = None
        self.appId = None
        self.desired = None
        self.timestamp = None

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

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

    def getDesired(self):
        return self.desired

    def setDesired(self, desired):
        self.desired = desired

    def getTimestamp(self):
        return self.timestamp

    def setTimestamp(self, timestamp):
        self.timestamp = timestamp
