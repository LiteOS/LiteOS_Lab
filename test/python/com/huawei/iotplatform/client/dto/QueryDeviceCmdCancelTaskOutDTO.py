from com.huawei.iotplatform.client.dto.DeviceCommandCancelTaskRespV4 import DeviceCommandCancelTaskRespV4
from com.huawei.iotplatform.client.dto.Pagination import Pagination


class QueryDeviceCmdCancelTaskOutDTO(object):
    pagination = Pagination()
    data = DeviceCommandCancelTaskRespV4()

    def __init__(self):
        pass

    def getPagination(self):
        return self.pagination

    def setPagination(self, pagination):
        self.pagination = pagination

    def getData(self):
        return self.data

    def setData(self, data):
        self.data = data
