class QueryUpgradeTaskListInDTO(object):

    def __init__(self):
        self.operationType = None
        self.operationStatus = None
        self.deviceType = None
        self.model = None
        self.manufacturerName = None
        self.deviceId = None
        self.pageNo = None
        self.pageSize = None

    def getOperationType(self):
        return self.operationType

    def setOperationType(self, operationType):
        self.operationType = operationType

    def getOperationStatus(self):
        return self.operationStatus

    def setOperationStatus(self, operationStatus):
        self.operationStatus = operationStatus

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

    def getModel(self):
        return self.model

    def setModel(self, model):
        self.model = model

    def getManufacturerName(self):
        return self.manufacturerName

    def setManufacturerName(self, manufacturerName):
        self.manufacturerName = manufacturerName

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize
