class DeviceServiceB(object):

    def __init__(self):
        self.serviceId = None
        self.reportedProps = None
        self.desiredProps = None
        self.eventTime = None
        self.serviceType = None

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getReportedProps(self):
        return self.reportedProps

    def setReportedProps(self, reportedProps):
        self.reportedProps = reportedProps

    def getDesiredProps(self):
        return self.desiredProps

    def setDesiredProps(self, desiredProps):
        self.desiredProps = desiredProps

    def getEventTime(self):
        return self.eventTime

    def setEventTime(self, eventTime):
        self.eventTime = eventTime

    def getServiceType(self):
        return self.serviceType

    def setServiceType(self, serviceType):
        self.serviceType = serviceType
