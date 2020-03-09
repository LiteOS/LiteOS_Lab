class Pagination(object):

    def __init__(self):
        self.pageNo = None
        self.pageSize = None
        self.totalSize = None

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getTotalSize(self):
        return self.totalSize

    def setTotalSize(self, totalSize):
        self.totalSize = totalSize
