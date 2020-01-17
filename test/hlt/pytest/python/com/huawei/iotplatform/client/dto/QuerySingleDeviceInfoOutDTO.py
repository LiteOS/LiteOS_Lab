from com.huawei.iotplatform.client.dto.DeviceInfo import DeviceInfo
from com.huawei.iotplatform.client.dto.DeviceService import DeviceService


class QuerySingleDeviceInfoOutDTO(object):
    deviceInfo = DeviceInfo()
    services = DeviceService()

    def __init__(self):
        self.deviceId = None
        self.gatewayId = None
        self.nodeType = None
        self.createTime = None
        self.lastModifiedTime = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getGatewayId(self):
        return self.gatewayId

    def setGatewayId(self, gatewayId):
        self.gatewayId = gatewayId

    def getNodeType(self):
        return self.nodeType

    def setNodeType(self, nodeType):
        self.nodeType = nodeType

    def getCreateTime(self):
        return self.createTime

    def setCreateTime(self, createTime):
        self.createTime = createTime

    def getLastModifiedTime(self):
        return self.lastModifiedTime

    def setLastModifiedTime(self, lastModifiedTime):
        self.lastModifiedTime = lastModifiedTime

    def getDeviceInfo(self):
        return self.deviceInfo

    def setDeviceInfo(self, deviceInfo):
        self.deviceInfo = deviceInfo

    def getServices(self):
        return self.services

    def setServices(self, services):
        self.services = services
