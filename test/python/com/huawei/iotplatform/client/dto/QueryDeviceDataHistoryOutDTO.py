from com.huawei.iotplatform.client.dto.DeviceDataHistoryDTO import DeviceDataHistoryDTO


class QueryDeviceDataHistoryOutDTO(object):
    deviceDataHistoryDTOs = DeviceDataHistoryDTO()

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

    def getDeviceDataHistoryDTOs(self):
        return self.deviceDataHistoryDTOs

    def setDeviceDataHistoryDTOs(self, deviceDataHistoryDTOs):
        self.deviceDataHistoryDTOs = deviceDataHistoryDTOs
