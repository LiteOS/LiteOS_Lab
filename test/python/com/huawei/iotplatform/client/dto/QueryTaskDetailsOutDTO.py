from com.huawei.iotplatform.client.dto.QueryTaskDetailDTO import QueryTaskDetailDTOCloud2NA


class QueryTaskDetailsOutDTO(object):
    taskDetails = QueryTaskDetailDTOCloud2NA()

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

    def getTaskDetails(self):
        return self.taskDetails

    def setTaskDetails(self, taskDetails):
        self.taskDetails = taskDetails
