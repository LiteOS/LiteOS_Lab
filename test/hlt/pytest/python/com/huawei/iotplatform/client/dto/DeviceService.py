class DeviceService(object):

    def __init__(self):
        self.serviceId = "serviceId"
        self.serviceType = "serviceType"
        self.data = "data"
        self.eventTime = "eventTime"

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getServiceType(self):
        return self.serviceType

    def setServiceType(self, serviceType):
        self.serviceType = serviceType

    def getData(self):
        return self.data

    def setData(self, data):
        self.data = data

    def getEventTime(self):
        return self.eventTime

    def setEventTime(self, eventTime):
        self.eventTime = eventTime
