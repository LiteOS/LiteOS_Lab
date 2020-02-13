class ClientInfo(object):

    def __init__(self):
        self.platformIp = None
        self.platformPort = None
        self.appId = None
        self.secret = None

    def getPlatformIp(self):
        return self.platformIp

    def setPlatformIp(self, platformIp):
        self.platformIp = platformIp

    def getPlatformPort(self):
        return self.platformPort

    def setPlatformPort(self, platformPort):
        self.platformPort = platformPort

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getSecret(self):
        return self.secret

    def setSecret(self, secret):
        self.secret = secret