class QueryBatchDevicesInfoInDTO(object):

    def __init__(self):
        self.appId = None
        self.gatewayId = None
        self.status = None
        self.nodeType = None
        self.deviceType = None
        self.pageNo = None
        self.pageSize = None
        self.startTime = None
        self.endTime = None
        self.sort = None
        self.select = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getGatewayId(self):
        return self.gatewayId

    def setGatewayId(self, gatewayId):
        self.gatewayId = gatewayId

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getNodeType(self):
        return self.nodeType

    def setNodeType(self, nodeType):
        self.nodeType = nodeType

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

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

    def getSort(self):
        return self.sort

    def setSort(self, sort):
        self.sort = sort

    def getSelect(self):
        return self.select

    def setSelect(self, select):
        self.select = select
