from com.huawei.iotplatform.client.dto.DeviceInfo import DeviceInfo


class NotifyDeviceAddedDTO(object):
    deviceInfo = DeviceInfo()

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.gatewayId = None
        self.nodeType = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

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

    def getDeviceInfo(self):
        return self.deviceInfo

    def setDeviceInfo(self, deviceInfo):
        self.deviceInfo = deviceInfo
