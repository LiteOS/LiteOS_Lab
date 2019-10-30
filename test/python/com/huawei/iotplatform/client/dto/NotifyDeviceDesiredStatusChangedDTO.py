class NotifyDeviceDesiredStatusChangedDTO(object):

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.serviceId = None
        self.properties = None
        self.status = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getProperties(self):
        return self.properties

    def setProperties(self, properties):
        self.properties = properties

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status
