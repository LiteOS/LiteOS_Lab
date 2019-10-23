class QueryDeviceGroupMembersInDTO(object):

    def __init__(self):
        self.devGroupId = None
        self.accessAppId = None
        self.pageNo = None
        self.pageSize = None

    def getDevGroupId(self):
        return self.devGroupId

    def setDevGroupId(self, devGroupId):
        self.devGroupId = devGroupId

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
