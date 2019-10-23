class QueryUpgradeSubTaskInDTO(object):

    def __init__(self):
        self.subOperationStatus = None
        self.pageNo = None
        self.pageSize = None

    def getSubOperationStatus(self):
        return self.subOperationStatus

    def setSubOperationStatus(self, subOperationStatus):
        self.subOperationStatus = subOperationStatus

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize
