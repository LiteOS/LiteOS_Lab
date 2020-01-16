class QueryUpgradePackageOutDTO(object):

    def __init__(self):
        self.fileId = None
        self.name = None
        self.version = None
        self.fileType = None
        self.deviceType = None
        self.model = None
        self.manufacturerName = None
        self.protocolType = None
        self.description = None
        self.date = None
        self.uploadTime = None

    def getFileId(self):
        return self.fileId

    def setFileId(self, fileId):
        self.fileId = fileId

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getVersion(self):
        return self.version

    def setVersion(self, version):
        self.version = version

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

    def getProtocolType(self):
        return self.protocolType

    def setProtocolType(self, protocolType):
        self.protocolType = protocolType

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getDate(self):
        return self.date

    def setDate(self, date):
        self.date = date

    def getUploadTime(self):
        return self.uploadTime

    def setUploadTime(self, uploadTime):
        self.uploadTime = uploadTime
