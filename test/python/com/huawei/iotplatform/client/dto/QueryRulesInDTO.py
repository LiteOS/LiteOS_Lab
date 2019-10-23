class QueryRulesInDTO(object):

    def __init__(self):
        self.name = None
        self.author = None
        self.appId = None
        self.select = None

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getAuthor(self):
        return self.author

    def setAuthor(self, author):
        self.author = author

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getSelect(self):
        return self.select

    def setSelect(self, select):
        self.select = select
