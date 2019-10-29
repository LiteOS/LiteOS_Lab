from com.huawei.iotplatform.client.dto.CustomField import CustomField
from com.huawei.iotplatform.client.dto.DeviceInfoDTO import DeviceInfoDTO
from com.huawei.iotplatform.client.dto.DeviceInfoDTO2 import DeviceInfoDTO2
from com.huawei.iotplatform.client.dto.Location import Location
from com.huawei.iotplatform.client.dto.TagDTO2 import TagDTO2


class RegDirectDeviceInDTO(object):
    customFields = CustomField
    deviceInfo = DeviceInfoDTO
    deviceInfo2 = DeviceInfoDTO2
    location = Location
    tags = TagDTO2

    def __init__(self):
        self.verifyCode = None
        self.nodeId = None
        self.endUserId = None
        self.psk = None
        self.timeout = None
        self.deviceName = None
        self.groupId = None
        self.imsi = None
        self.isSecure = None
        self.productId = None
        self.account = None

    def getVerifyCode(self):
        return self.verifyCode

    def setVerifyCode(self, verifyCode):
        self.verifyCode = verifyCode

    def getNodeId(self):
        return self.nodeId

    def setNodeId(self, nodeId):
        self.nodeId = nodeId

    def getEndUserId(self):
        return self.endUserId

    def setEndUserId(self, endUserId):
        self.endUserId = endUserId

    def getPsk(self):
        return self.psk

    def setPsk(self, psk):
        self.psk = psk

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout

    def getDeviceName(self):
        return self.deviceName

    def setDeviceName(self, deviceName):
        self.deviceName = deviceName

    def getGroupId(self):
        return self.groupId

    def setGroupId(self, groupId):
        self.groupId = groupId

    def getImsi(self):
        return self.imsi

    def setImsi(self, imsi):
        self.imsi = imsi

    def getIsSecure(self):
        return self.isSecure

    def setIsSecure(self, isSecure):
        self.isSecure = isSecure

    def getProductId(self):
        return self.productId

    def setProductId(self, productId):
        self.productId = productId

    def getAccount(self):
        return self.account

    def setAccount(self, account):
        self.account = account

    def getCustomFields(self):
        return self.customFields

    def setCustomFields(self, customFields):
        self.customFields = customFields

    def getDeviceInfo(self):
        return self.deviceInfo

    def setDeviceInfo(self, deviceInfo):
        self.deviceInfo = deviceInfo

    def getDeviceInfo2(self):
        return self.deviceInfo2

    def setDeviceInfo2(self, deviceInfo2):
        self.deviceInfo2 = deviceInfo2

    def getLocation(self):
        return self.location

    def setLocation(self, location):
        self.location = location

    def getTags(self):
        return self.tags

    def setTags(self, tags):
        self.tags = tags
