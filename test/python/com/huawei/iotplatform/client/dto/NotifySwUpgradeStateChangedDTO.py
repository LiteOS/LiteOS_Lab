class NotifySwUpgradeStateChangedDTO(object):

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.appId = None
        self.operationId = None
        self.subOperationId = None
        self.swUpgradeState = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getOperationId(self):
        return self.operationId

    def setOperationId(self, operationId):
        self.operationId = operationId

    def getSubOperationId(self):
        return self.subOperationId

    def setSubOperationId(self, subOperationId):
        self.subOperationId = subOperationId

    def getSwUpgradeState(self):
        return self.swUpgradeState

    def setSwUpgradeState(self, swUpgradeState):
        self.swUpgradeState = swUpgradeState
