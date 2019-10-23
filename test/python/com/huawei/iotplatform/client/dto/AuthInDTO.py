class AuthInDTO(object):

    def __init__(self):
        self.appId = None
        self.secret = None

    def getAppId(self):
        return self.appId

    def setAppId(self, appId):
        self.appId = appId

    def getSecret(self):
        return self.secret

    def setSecret(self, secret):
        self.secret = secret
