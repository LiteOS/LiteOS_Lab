class QueryBatchSubInDTO(object):

    def __init__(self):
        self.appId = None
        self.notifyType = None
        self.pageNo = None
        self.pageSize = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize
