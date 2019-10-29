class QuerySingleDeviceGroupOutDTO(object):

    def __init__(self):
        self.name = None
        self.description = None
        self.id = None
        self.appId = None
        self.maxDevNum = None
        self.curDevNum = None
        self.creator = None

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getId(self):
        return self.id

    def setId(self, id):
        self.id = id

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getMaxDevNum(self):
        return self.maxDevNum

    def setMaxDevNum(self, maxDevNum):
        self.maxDevNum = maxDevNum

    def getCurDevNum(self):
        return self.curDevNum

    def setCurDevNum(self, curDevNum):
        self.curDevNum = curDevNum

    def getCreator(self):
        return self.creator

    def setCreator(self, creator):
        self.creator = creator
