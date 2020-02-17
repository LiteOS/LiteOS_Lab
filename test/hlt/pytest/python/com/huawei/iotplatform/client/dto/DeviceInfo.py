class DeviceInfo(object):

    def __init__(self):
        self.nodeId = "nodeId"
        self.name = "name"
        self.description = "description"
        self.manufacturerId = "manufacturerId"
        self.manufacturerName = "manufacturerName"
        self.mac = "mac"
        self.location = "location"
        self.deviceType = "deviceType"
        self.model = "model"
        self.swVersion = "swVersion"
        self.fwVersion = "fwVersion"
        self.hwVersion = "hwVersion"
        self.protocolType = "protocolType"
        self.bridgeId = "bridgeId"
        self.status = "status"
        self.statusDetail = "statusDetail"
        self.mute = "mute"
        self.supportedSecurity = "supportedSecurity"
        self.isSecurity = "isSecurity"
        self.signalStrength = "signalStrength"
        self.sigVersion = "sigVersion"
        self.serialNumber = "serialNumber"
        self.batteryLevel = "batteryLevel"

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

    def getManufacturerName(self):
        return self.manufacturerName

    def setManufacturerName(self, manufacturerName):
        self.manufacturerName = manufacturerName

    def getMac(self):
        return self.mac

    def setMac(self, mac):
        self.mac = mac

    def getLocation(self):
        return self.location

    def setLocation(self, location):
        self.location = location

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

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
