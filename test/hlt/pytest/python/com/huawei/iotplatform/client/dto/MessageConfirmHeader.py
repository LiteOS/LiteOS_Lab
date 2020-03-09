class MessageConfirmHeader(object):

    def __init__(self):
        self.requestId = "requestId"
        self.from_ = "from_"
        self.to = "to"
        self.status = "status"
        self.timestamp = "timestamp"

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

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getTimestamp(self):
        return self.timestamp

    def setTimestamp(self, timestamp):
        self.timestamp = timestamp
