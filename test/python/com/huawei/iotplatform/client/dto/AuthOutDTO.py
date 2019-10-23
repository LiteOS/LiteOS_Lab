class AuthOutDTO(object):

    # def __init__(self,scope,tokenType,expiresIn,accessToken,refreshToken):
    #     self.scope = scope
    #     self.tokenType = tokenType
    #     self.expiresIn = expiresIn
    #     self.accessToken = accessToken
    #     self.refreshToken = refreshToken
    def __init__(self):
        self.scope = None
        self.tokenType = None
        self.expiresIn = None
        self.accessToken = None
        self.refreshToken = None

    def getScope(self):
        return self.scope

    def setScope(self, scope):
        self.scope = scope

    def getTokenType(self):
        return self.tokenType

    def setTokenType(self, tokenType):
        self.tokenType = tokenType

    def getExpiresIn(self):
        return self.expiresIn

    def setEexpiresIn(self, expiresIn):
        self.expiresIn = expiresIn

    def getAccessToken(self):
        return self.accessToken

    def setAccessToken(self, accessToken):
        self.accessToken = accessToken

    def getRefreshToken(self):
        return self.refreshToken

    def setRefreshToken(self, refreshToken):
        self.refreshToken = refreshToken
