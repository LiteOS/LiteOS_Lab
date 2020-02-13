class CommandRspHeader(object):

    def __init__(self):
        self.requestId = "requestId"
        self.from_ = "from_"
        self.to = "to"
        self.deviceId = "deviceId"
        self.serviceType = "serviceType"
        self.method = "method"

    def getRequestId(self):
        return self.requestId

    def setRequestId(self, requestId):
        self.requestId = requestId

    def getFrom(self):
        return self.from_

    def setFrom(self, from_):
        self.from_ = from_

    def getTo(self):
        return self.to

    def setTo(self, to):
        self.to = to

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getServiceType(self):
        return self.serviceType

    def setServiceType(self, serviceType):
        self.serviceType = serviceType

    def getMethod(self):
        return self.method

    def setMethod(self, method):
        self.method = method
