class OperatePolicy(object):

    def __init__(self):
        self.executeType = None
        self.startTime = None
        self.endTime = None
        self.retryType = None
        self.retryTimes = None

    def getExecuteType(self):
        return self.executeType

    def setExecuteType(self, executeType):
        self.executeType = executeType

    def getStartTime(self):
        return self.startTime

    def setStartTime(self, startTime):
        self.startTime = startTime

    def getEndTime(self):
        return self.endTime

    def setEndTime(self, endTime):
        self.endTime = endTime

    def getRetryType(self):
        return self.retryType

    def setRetryType(self, retryType):
        self.retryType = retryType

    def getRetryTimes(self):
        return self.retryTimes

    def setRetryTimes(self, retryTimes):
        self.retryTimes = retryTimes
