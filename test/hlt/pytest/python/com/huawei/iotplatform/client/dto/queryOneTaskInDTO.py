class queryOneTaskInDTO(object):

    def __init__(self):
        self.appId = None
        self.select = None
        self.taskId = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getSelect(self):
        return self.select

    def setSelect(self, select):
        self.select = select

    def getTaskId(self):
        return self.taskId

    def setTaskId(self, taskId):
        self.taskId = taskId
