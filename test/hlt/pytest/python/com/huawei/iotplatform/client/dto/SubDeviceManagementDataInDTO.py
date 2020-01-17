class SubDeviceManagementDataInDTO(object):

    def __init__(self):
        self.notifyType = None
        self.callbackurl = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getCallbackurl(self):
        return self.callbackurl

    def setCallbackurl(self, callbackurl):
        self.callbackurl = callbackurl
