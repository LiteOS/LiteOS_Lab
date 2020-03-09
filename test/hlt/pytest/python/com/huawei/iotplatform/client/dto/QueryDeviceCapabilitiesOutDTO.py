from com.huawei.iotplatform.client.dto.DeviceCapabilityDTO import DeviceCapabilityDTO


class QueryDeviceCapabilitiesOutDTO(object):
    deviceCapabilities = DeviceCapabilityDTO()

    def __init__(self):
        pass

    def getDeviceCapabilities(self):
        return self.deviceCapabilities

    def setDeviceCapabilities(self, deviceCapabilities):
        self.deviceCapabilities = deviceCapabilities
