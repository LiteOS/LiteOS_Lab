class DeviceInfoDTO(object):

    def __init__(self):
        self.nodeId = None
        self.name = None
        self.description = None
        self.manufacturerId = None
        self.manufacturerName = None
        self.mac = None
        self.location = None
        self.model = None
        self.swVersion = None
        self.fwVersion = None
        self.hwVersion = None
        self.protocolType = None
        self.bridgeId = None
        self.status = None
        self.statusDetail = None
        self.mute = None
        self.supportedSecurity = None
        self.isSecurity = None
        self.signalStrength = None
        self.sigVersion = None
        self.serialNumber = None
        self.batteryLevel = None

    def getNodeId(self):
        return self.nodeId

    def setNodeId(self, nodeId):
        self.nodeId = nodeId

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getManufacturerId(self):
        return self.manufacturerId

    def setManufacturerId(self, manufacturerId):
        self.manufacturerId = manufacturerId

    def getMac(self):
        return self.mac

    def setMac(self, mac):
        self.mac = mac

    def getLocation(self):
        return self.location

    def setLocation(self, location):
        self.location = location

    def getModel(self):
        return self.model

    def setModel(self, model):
        self.model = model

    def getSwVersion(self):
        return self.swVersion

    def setSwVersion(self, swVersion):
        self.swVersion = swVersion

    def getFwVersion(self):
        return self.fwVersion

    def setFwVersion(self, fwVersion):
        self.fwVersion = fwVersion

    def getHwVersion(self):
        return self.hwVersion

    def setHwVersion(self, hwVersion):
        self.hwVersion = hwVersion

    def getProtocolType(self):
        return self.protocolType

    def setProtocolType(self, protocolType):
        self.protocolType = protocolType

    def getBridgeId(self):
        return self.bridgeId

    def setBridgeId(self, bridgeId):
        self.bridgeId = bridgeId

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getStatusDetail(self):
        return self.statusDetail

    def setStatusDetail(self, statusDetail):
        self.statusDetail = statusDetail

    def getMute(self):
        return self.mute

    def setMute(self, mute):
        self.mute = mute

    def getSupportedSecurity(self):
        return self.supportedSecurity

    def setSupportedSecurity(self, supportedSecurity):
        self.supportedSecurity = supportedSecurity

    def getIsSecurity(self):
        return self.isSecurity

    def setIsSecurity(self, isSecurity):
        self.isSecurity = isSecurity

    def getSignalStrength(self):
        return self.signalStrength

    def setSignalStrength(self, signalStrength):
        self.signalStrength = signalStrength

    def getSigVersion(self):
        return self.sigVersion

    def setSigVersion(self, sigVersion):
        self.sigVersion = sigVersion

    def getSerialNumber(self):
        return self.serialNumber

    def setSerialNumber(self, serialNumber):
        self.serialNumber = serialNumber

    def getBatteryLevel(self):
        return self.batteryLevel

    def setBatteryLevel(self, batteryLevel):
        self.batteryLevel = batteryLevel
