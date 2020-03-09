class CreateDeviceGroupInDTO(object):

    def __init__(self):
        self.name = None
        self.description = None
        self.appId = None
        self.maxDevNum = None
        self.deviceIds = None
        self.id = None

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getMaxDevNum(self):
        return self.maxDevNum

    def setMaxDevNum(self, maxDevNum):
        self.maxDevNum = maxDevNum

    def getDeviceIds(self):
        return self.deviceIds

    def setDeviceIds(self, deviceIds):
        self.deviceIds = deviceIds

    def getId(self):
        return self.id

    def setId(self, id):
        self.id = id
