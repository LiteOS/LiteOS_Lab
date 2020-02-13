class RegDirectDeviceOutDTO(object):

    def __init__(self):
        self.deviceId = None
        self.verifyCode = None
        self.timeout = None
        self.psk = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getVerifyCode(self):
        return self.verifyCode

    def setVerifyCode(self, verifyCode):
        self.verifyCode = verifyCode

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout

    def getPsk(self):
        return self.psk

    def setPsk(self, psk):
        self.psk = psk
