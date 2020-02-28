class QueryDeviceStatusOutDTO(object):

    def __init__(self):
        self.deviceId = None
        self.activated = None
        self.name = None

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getActivated(self):
        return self.activated

    def setActivated(self, activated):
        self.activated = activated

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name
