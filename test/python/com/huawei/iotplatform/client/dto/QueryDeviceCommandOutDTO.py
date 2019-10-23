from com.huawei.iotplatform.client.dto.DeviceCommandRespV4 import DeviceCommandRespV4
from com.huawei.iotplatform.client.dto.Pagination import Pagination


class QueryDeviceCommandOutDTO(object):
    pagination = Pagination()
    data = DeviceCommandRespV4()

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
