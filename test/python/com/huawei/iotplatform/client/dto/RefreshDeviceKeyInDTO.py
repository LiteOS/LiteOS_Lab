class RefreshDeviceKeyInDTO(object):

    def __init__(self):
        self.deviceId = None
        self.verifyCode = None
        self.nodeId = None
        self.timeout = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getVerifyCode(self):
        return self.verifyCode

    def setVerifyCode(self, verifyCode):
        self.verifyCode = verifyCode

    def getNodeId(self):
        return self.nodeId

    def setNodeId(self, nodeId):
        self.nodeId = nodeId

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout
