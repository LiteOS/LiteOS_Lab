from com.huawei.iotplatform.client.dto.DeviceService import DeviceService


class NotifyDeviceDatasChangedDTO(object):

    def __init__(self):
        self.notifyType = None
        self.requestId = None
        self.deviceId = None
        self.gatewayId = None
        self.service = DeviceService()

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getRequestId(self):
        return self.requestId

    def setRequestId(self, requestId):
        self.requestId = requestId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getGatewayId(self):
        return self.gatewayId

    def setGatewayId(self, gatewayId):
        self.gatewayId = gatewayId

    def getService(self):
        return self.service

    def setService(self, service):
        self.service = service