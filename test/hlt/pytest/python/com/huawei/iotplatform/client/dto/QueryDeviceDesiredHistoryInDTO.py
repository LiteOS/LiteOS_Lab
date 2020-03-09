class QueryDeviceDesiredHistoryInDTO(object):

    def __init__(self):
        self.appId = None
        self.deviceId = None
        self.gatewayId = None
        self.serviceId = None
        self.property = None
        self.pageNo = None
        self.pageSize = None
        self.startTime = None
        self.endTime = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getGatewayId(self):
        return self.gatewayId

    def setGatewayId(self, gatewayId):
        self.gatewayId = gatewayId

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getProperty(self):
        return self.property

    def setProperty(self, property):
        self.property = property

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getStartTime(self):
        return self.startTime

    def setStartTime(self, startTime):
        self.startTime = startTime

    def getEndTime(self):
        return self.endTime

    def setEndTime(self, endTime):
        self.endTime = endTime
