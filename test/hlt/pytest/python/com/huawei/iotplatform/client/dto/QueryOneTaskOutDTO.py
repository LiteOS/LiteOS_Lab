from com.huawei.iotplatform.client.dto.ObjectNode import ObjectNode
from com.huawei.iotplatform.client.dto.TagDTO import TagDTO


class QueryOneTaskOutDTO(object):
    param = ObjectNode
    tags = TagDTO

    def __init__(self):
        self.taskId = None
        self.taskName = None
        self.appId = None
        self.operator = None
        self.taskFrom = None
        self.taskType = None
        self.status = None
        self.startTime = None
        self.timeout = None
        self.progress = None
        self.totalCnt = None
        self.successCnt = None
        self.successRate = None
        self.failCnt = None
        self.timeoutCnt = None
        self.expiredCnt = None
        self.completeCnt = None

    def getTaskId(self):
        return self.taskId

    def setTaskId(self, taskId):
        self.taskId = taskId

    def getTaskName(self):
        return self.taskName

    def setTaskName(self, taskName):
        self.taskName = taskName

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getOperator(self):
        return self.operator

    def setOperator(self, operator):
        self.operator = operator

    def getTaskFrom(self):
        return self.taskFrom

    def setTaskFrom(self, taskFrom):
        self.taskFrom = taskFrom

    def getTaskType(self):
        return self.taskType

    def setTaskType(self, taskType):
        self.taskType = taskType

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getStartTime(self):
        return self.startTime

    def setStartTime(self, startTime):
        self.startTime = startTime

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout

    def getProgress(self):
        return self.progress

    def setProgress(self, progress):
        self.progress = progress

    def getTotalCnt(self):
        return self.totalCnt

    def setTotalCnt(self, totalCnt):
        self.totalCnt = totalCnt

    def getSuccessCnt(self):
        return self.successCnt

    def setSuccessCnt(self, successCnt):
        self.successCnt = successCnt

    def getSuccessRate(self):
        return self.successRate

    def setSuccessRate(self, successRate):
        self.successRate = successRate

    def getFailCnt(self):
        return self.failCnt

    def setFailCnt(self, failCnt):
        self.failCnt = failCnt

    def getTimeoutCnt(self):
        return self.timeoutCnt

    def setTimeoutCnt(self, timeoutCnt):
        self.timeoutCnt = timeoutCnt

    def getExpiredCnt(self):
        return self.expiredCnt

    def setExpiredCnt(self, expiredCnt):
        self.expiredCnt = expiredCnt

    def getCompleteCnt(self):
        return self.completeCnt

    def setCompleteCnt(self, completeCnt):
        self.completeCnt = completeCnt

    def getParam(self):
        return self.param

    def setParam(self, param):
        self.param = param

    def getTags(self):
        return self.tags

    def setTags(self, tags):
        self.tags = tags
