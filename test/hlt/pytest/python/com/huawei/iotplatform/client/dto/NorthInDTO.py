class NorthInDTO(object):

    def __init__(self):
        self.platformIp = None
        self.platformPort = None

    def getPlatformIp(self):
        return self.platformIp

    def setPlatformIp(self, platformIp):
        self.platformIp = platformIp

    def getPlatformPort(self):
        return self.platformPort

    def setPlatformPort(self, platformPort):
        self.platformPort = platformPort
