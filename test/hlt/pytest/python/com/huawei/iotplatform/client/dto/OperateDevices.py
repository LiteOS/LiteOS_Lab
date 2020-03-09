class OperateDevices(object):

    def __init__(self):
        self.deviceGroups = None
        self.deviceType = None
        self.model = None
        self.manufacturerName = None
        self.devices = None

    def getDeviceGroups(self):
        return self.deviceGroups

    def setDeviceGroups(self, deviceGroups):
        self.deviceGroups = deviceGroups

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

    def getModel(self):
        return self.model

    def setModel(self, model):
        self.model = model

    def getManufacturerName(self):
        return self.manufacturerName

    def setManufacturerId(self, manufacturerName):
        self.manufacturerName = manufacturerName

    def getDevices(self):
        return self.devices

    def setDevices(self, devices):
        self.devices = devices
