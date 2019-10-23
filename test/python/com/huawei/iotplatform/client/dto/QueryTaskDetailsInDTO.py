class QueryTaskDetailsInDTO(object):

    def __init__(self):
        self.appId = None
        self.taskId = None
        self.status = None
        self.index = None
        self.nodeId = None
        self.deviceId = None
        self.commandId = None
        self.pageNo = None
        self.pageSize = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getTaskId(self):
        return self.taskId

    def setTaskId(self, taskId):
        self.taskId = taskId

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getIndex(self):
        return self.index

    def setIndex(self, index):
        self.index = index

    def getNodeId(self):
        return self.nodeId

    def setNodeId(self, nodeId):
        self.nodeId = nodeId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getCommandId(self):
        return self.commandId

    def setCommandId(self, commandId):
        self.commandId = commandId

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize