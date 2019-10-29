from com.huawei.iotplatform.client.dto.ServiceCapabilityDTO import ServiceCapabilityDTO


class DeviceCapabilityDTO(object):
    serviceCapabilities = ServiceCapabilityDTO()

    def __init__(self):
        self.deviceId = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getServiceCapabilities(self):
        return self.serviceCapabilities

    def setServiceCapabilities(self, serviceCapabilities):
        self.serviceCapabilities = serviceCapabilities
