class NotifySwUpgradeResultDTO(object):

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.appId = None
        self.operationId = None
        self.subOperationId = None
        self.curVersion = None
        self.targetVersion = None
        self.sourceVersion = None
        self.swUpgradeResult = None
        self.upgradeTime = None
        self.resultDesc = None
        self.errorCode = None
        self.description = None

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

    def getSwUpgradeResult(self):
        return self.swUpgradeResult

    def setSwUpgradeResult(self, swUpgradeResult):
        self.swUpgradeResult = swUpgradeResult

    def getUpgradeTime(self):
        return self.upgradeTime

    def setUpgradeTime(self, upgradeTime):
        self.upgradeTime = upgradeTime

    def getResultDesc(self):
        return self.resultDesc

    def setResultDesc(self, resultDesc):
        self.resultDesc = resultDesc

    def getErrorCode(self):
        return self.errorCode

    def setErrorCode(self, errorCode):
        self.errorCode = errorCode

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description
