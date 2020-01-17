class DeviceInfoDTO2(object):

    def __init__(self):
        self.region = None
        self.timezone = None
        self.activeServiceCount = None

    def getRegion(self):
        return self.region

    def setRegion(self, region):
        self.region = region

    def getTimezone(self):
        return self.timezone

    def setTimezone(self, timezone):
        self.timezone = timezone

    def getActiveServiceCount(self):
        return self.activeServiceCount

    def setActiveServiceCount(self, activeServiceCount):
        self.activeServiceCount = activeServiceCount
