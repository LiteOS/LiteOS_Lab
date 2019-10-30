class NotifyFwUpgradeResultDTO(object):

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.appId = None
        self.operationId = None
        self.subOperationId = None
        self.curVersion = None
        self.targetVersion = None
        self.sourceVersion = None
        self.Status = None
        self.statusDesc = None
        self.upgradeTime = None

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

    def getCurVersion(self):
        return self.curVersion

    def setCurVersion(self, curVersion):
        self.curVersion = curVersion

    def getTargetVersion(self):
        return self.targetVersion

    def setTargetVersion(self, targetVersion):
        self.targetVersion = targetVersion

    def getSourceVersion(self):
        return self.sourceVersion

    def setSourceVersion(self, sourceVersion):
        self.sourceVersion = sourceVersion

    def getStatus(self):
        return self.Status

    def setStatus(self, Status):
        self.Status = Status

    def getStatusDesc(self):
        return self.statusDesc

    def setStatusDesc(self, statusDesc):
        self.statusDesc = statusDesc

    def getUpgradeTime(self):
        return self.upgradeTime

    def setUpgradeTime(self, upgradeTime):
        self.upgradeTime = upgradeTime
