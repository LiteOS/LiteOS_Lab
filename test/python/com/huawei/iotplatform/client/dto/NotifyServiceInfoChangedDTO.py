from com.huawei.iotplatform.client.dto.ServiceInfo import ServiceInfo


class NotifyServiceInfoChangedDTO(object):
    serviceInfo = ServiceInfo()

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.gatewayId = None
        self.serviceId = None
        self.serviceType = None

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

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getServiceType(self):
        return self.serviceType

    def setServiceType(self, serviceType):
        self.serviceType = serviceType

    def getServiceInfo(self):
        return self.serviceInfo

    def setServiceInfo(self, serviceInfo):
        self.serviceInfo = serviceInfo
