class CommandNA2CloudHeader(object):
    mode = "mode"
    method = "method"
    requestId = "requestId"
    from_ = "from"
    to = "to"
    toType = "toType"
    callbackURL = "callbackURL"

    def __init__(self):
        # self.requestId = "requestId"
        # self.from_ = "from"
        # self.to = "to"
        # self.toType = "toType"
        # self.callbackURL = "callbackURL"
        pass

    def getRequestId(self):
        return self.requestId

    def setRequestId(self, requestId):
        self.requestId = requestId

    def getMode(self):
        return self.mode

    def setMode(self, mode):
        self.mode = mode

    def getFrom(self):
        return self.from_

    def setFrom(self, from_):
        self.from_ = from_

    def getTo(self):
        return self.to

    def setTo(self, to):
        self.to = to

    def getToType(self):
        return self.toType

    def setToType(self, toType):
        self.toType = toType

    def getMethod(self):
        return self.method

    def setMethod(self, method):
        self.method = method

    def getCallbackURL(self):
        return self.callbackURL

    def setCallbackURL(self, callbackURL):
        self.callbackURL = callbackURL
