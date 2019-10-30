class DeleteBatchSubInDTO(object):

    def __init__(self):
        self.appId = None
        self.notifyType = None
        self.callbackUrl = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getCallbackUrl(self):
        return self.callbackUrl

    def setCallbackUrl(self, callbackUrl):
        self.callbackUrl = callbackUrl
