from com.huawei.iotplatform.client.dto.OperationInfo import OperationInfo


class QueryUpgradeTaskListOutDTO(object):
    data = OperationInfo()

    def __init__(self):
        self.pageNo = None
        self.pageSize = None
        self.totalCount = None

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getTotalCount(self):
        return self.totalCount

    def setTotalCount(self, totalCount):
        self.totalCount = totalCount

    def getData(self):
        return self.data

    def setData(self, data):
        self.data = data
