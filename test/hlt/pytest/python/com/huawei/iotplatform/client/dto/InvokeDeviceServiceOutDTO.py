class InvokeDeviceServiceOutDTO(object):

    def __init__(self):
        self.status = None
        self.timestamp = None
        self.requestId = None

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getTimestamp(self):
        return self.timestamp

    def setTimestamp(self, timestamp):
        self.timestamp = timestamp

    def getRequestId(self):
        return self.requestId

    def setRequestId(self, requestId):
        self.requestId = requestId
