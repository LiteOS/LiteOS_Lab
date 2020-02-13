class SubscriptionDTO(object):

    def __init__(self):
        self.subscriptionId = None
        self.notifyType = None
        self.callbackurl = None

    def getSubscriptionId(self):
        return self.subscriptionId

    def setSubscriptionId(self, subscriptionId):
        self.subscriptionId = subscriptionId

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getCallbackurl(self):
        return self.callbackurl

    def setCallbackurl(self, callbackurl):
        self.callbackurl = callbackurl
