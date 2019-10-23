class QueryDeviceGroupsInDTO(object):

    def __init__(self):
        self.accessAppId = None
        self.pageNo = None
        self.pageSize = None
        self.name = None

    def getAccessAppId(self):
        return self.accessAppId

    def setAccessAppId(self, accessAppId):
        self.accessAppId = accessAppId

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name
