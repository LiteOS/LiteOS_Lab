class RefreshDeviceKeyOutDTO(object):

    def __init__(self):
        self.verifyCode = None
        self.timeout = None

    def getVerifyCode(self):
        return self.verifyCode

    def setVerifyCode(self, verifyCode):
        self.verifyCode = verifyCode

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout
