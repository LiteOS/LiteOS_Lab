class QueryDeviceCommandInDTO(object):

    def __init__(self):
        self.pageNo = None
        self.pageSize = None
        self.deviceId = None
        self.startTime = None
        self.endTime = None
        self.appId = None

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getStartTime(self):
        return self.startTime

    def setStartTime(self, startTime):
        self.startTime = startTime

    def getEndTime(self):
        return self.endTime

    def setEndTime(self, endTime):
        self.endTime = endTime

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId
