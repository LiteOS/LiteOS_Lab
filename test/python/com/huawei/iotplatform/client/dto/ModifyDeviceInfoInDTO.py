from com.huawei.iotplatform.client.dto.CustomField import CustomField
from com.huawei.iotplatform.client.dto.DeviceConfigDTO import DeviceConfigDTO
from com.huawei.iotplatform.client.dto.TagDTO2 import TagDTO2


class ModifyDeviceInfoInDTO(object):
    customFields = CustomField
    deviceConfig = DeviceConfigDTO
    tags = TagDTO2

    def __init__(self):
        self.deviceType = None
        self.endUser = None
        self.location = None
        self.manufacturerId = None
        self.manufacturerName = None
        self.model = None
        self.mute = None
        self.name = None
        self.organization = None
        self.protocolType = None
        self.region = None
        self.timezone = None
        self.imsi = None
        self.ip = None
        self.isSecure = None
        self.psk = None

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

    def getEndUser(self):
        return self.endUser

    def setEndUser(self, endUser):
        self.endUser = endUser

    def getLocation(self):
        return self.location

    def setLocation(self, location):
        self.location = location

    def getManufacturerId(self):
        return self.manufacturerId

    def setManufacturerId(self, manufacturerId):
        self.manufacturerId = manufacturerId

    def getManufacturerName(self):
        return self.manufacturerName

    def setManufacturerName(self, manufacturerName):
        self.manufacturerName = manufacturerName

    def getModel(self):
        return self.model

    def setModel(self, model):
        self.model = model

    def getMute(self):
        return self.mute

    def setMute(self, mute):
        self.mute = mute

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getOrganization(self):
        return self.organization

    def setOrganization(self, organization):
        self.organization = organization

    def getProtocolType(self):
        return self.protocolType

    def setProtocolType(self, protocolType):
        self.protocolType = protocolType

    def getRegion(self):
        return self.region

    def setRegion(self, region):
        self.region = region

    def getTimezone(self):
        return self.timezone

    def setTimezone(self, timezone):
        self.timezone = timezone

    def getImsi(self):
        return self.imsi

    def setImsi(self, imsi):
        self.imsi = imsi

    def getIp(self):
        return self.ip

    def setIp(self, ip):
        self.ip = ip

    def getIsSecure(self):
        return self.isSecure

    def setIsSecure(self, isSecure):
        self.isSecure = isSecure

    def getPsk(self):
        return self.psk

    def setPsk(self, psk):
        self.psk = psk

    def getCustomFields(self):
        return self.customFields

    def setCustomFields(self, customFields):
        self.customFields = customFields

    def getDeviceConfigDTO(self):
        return self.deviceConfigDTO

    def setDeviceConfigDTO(self, deviceConfigDTO):
        self.deviceConfigDTO = deviceConfigDTO

    def getTags(self):
        return self.tags

    def setTags(self, tags):
        self.tags = tags
