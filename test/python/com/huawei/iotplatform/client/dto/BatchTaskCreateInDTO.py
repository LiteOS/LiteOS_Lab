from com.huawei.iotplatform.client.dto.ObjectNode import ObjectNode
from com.huawei.iotplatform.client.dto.TagDTO2 import TagDTO2


class BatchTaskCreateInDTO(object):
    tags = TagDTO2
    param = ObjectNode

    def __init__(self):
        self.appId = None
        self.taskName = None
        self.taskType = None
        self.timeout = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getTaskName(self):
        return self.taskName

    def setTaskName(self, taskName):
        self.taskName = taskName

    def getTaskType(self):
        return self.taskType

    def setTaskType(self, taskType):
        self.taskType = taskType

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout

    def getTags(self):
        return self.tags

    def setTags(self, tags):
        self.tags = tags

    def getParam(self):
        return self.param

    def setParam(self, param):
        self.param = param
