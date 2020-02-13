class SubOperationInfo(object):

    def __init__(self):
        self.subOperationId = None
        self.createTime = None
        self.startTime = None
        self.stopTime = None
        self.operateType = None
        self.deviceId = None
        self.status = None
        self.detailInfo = None
        self.extendInfo = None

    def getSubOperationId(self):
        return self.subOperationId

    def setSubOperationId(self, subOperationId):
        self.subOperationId = subOperationId

    def getCreateTime(self):
        return self.createTime

    def setCreateTime(self, createTime):
        self.createTime = createTime

    def getStartTime(self):
        return self.startTime

    def setStartTime(self, startTime):
        self.startTime = startTime

    def getStopTime(self):
        return self.stopTime

    def setStopTime(self, stopTime):
        self.stopTime = stopTime

    def getOperateType(self):
        return self.operateType

    def setOperateType(self, operateType):
        self.operateType = operateType

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getDetailInfo(self):
        return self.detailInfo

    def setDetailInfo(self, detailInfo):
        self.detailInfo = detailInfo

    def getExtendInfo(self):
        return self.extendInfo

    def setExtendInfo(self, extendInfo):
        self.extendInfo = extendInfo
