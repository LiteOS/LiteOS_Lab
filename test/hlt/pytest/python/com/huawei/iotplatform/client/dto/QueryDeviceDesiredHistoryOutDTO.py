from com.huawei.iotplatform.client.dto.DeviceDesiredHistoryDTO import DeviceDesiredHistoryDTO


class QueryDeviceDesiredHistoryOutDTO(object):
    DeviceDesiredHistoryDTO = DeviceDesiredHistoryDTO()

    def __init__(self):
        self.totalCount = None
        self.pageNo = None
        self.pageSize = None

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

    def getDeviceDesiredHistoryDTO(self):
        return self.DeviceDesiredHistoryDTO

    def setDeviceDesiredHistoryDTO(self, DeviceDesiredHistoryDTO):
        self.DeviceDesiredHistoryDTO = DeviceDesiredHistoryDTO
