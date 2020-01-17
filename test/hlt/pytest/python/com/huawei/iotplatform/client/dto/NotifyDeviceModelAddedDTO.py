class NotifyDeviceModelAddedDTO(object):

    def __init__(self):
        self.notifyType = None
        self.appId = None
        self.deviceType = None
        self.manufacturerName = None
        self.manufacturerId = None
        self.model = None
        self.protocolType = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

    def getManufacturerName(self):
        return self.manufacturerName

    def setManufacturerName(self, manufacturerName):
        self.manufacturerName = manufacturerName

    def getManufacturerId(self):
        return self.manufacturerId

    def setManufacturerId(self, manufacturerId):
        self.manufacturerId = manufacturerId

    def getModel(self):
        return self.model

    def setModel(self, model):
        self.model = model

    def getProtocolType(self):
        return self.protocolType

    def setProtocolType(self, protocolType):
        self.protocolType = protocolType
