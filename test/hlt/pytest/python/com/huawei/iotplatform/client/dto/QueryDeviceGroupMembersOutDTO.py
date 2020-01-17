class QueryDeviceGroupMembersOutDTO(object):

    def __init__(self):
        self.totalCount = None
        self.pageNo = None
        self.pageSize = None
        self.deviceIds = None

    def getTotalCount(self):
        return self.totalCount

    def setTotalCount(self, totalCount):
        self.totalCount = totalCount

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getDeviceIds(self):
        return self.deviceIds

    def setDeviceIds(self, deviceIds):
        self.deviceIds = deviceIds
