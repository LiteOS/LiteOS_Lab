class QueryUpgradePackageListInDTO(object):

    def __init__(self):
        self.fileType = None
        self.deviceType = None
        self.model = None
        self.manufacturerName = None
        self.version = None
        self.pageNo = None
        self.pageSize = None

    def getFileType(self):
        return self.fileType

    def setFileType(self, fileType):
        self.fileType = fileType

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

    def getVersion(self):
        return self.version

    def setVersion(self, version):
        self.version = version

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize
