from com.huawei.iotplatform.client.dto.CommandDTO import CommandDTO
from com.huawei.iotplatform.client.dto.TagDTO2 import TagDTO2


class ObjectNode(object):
    command = CommandDTO
    tags = TagDTO2

    def __init__(self):
        self.fileId = None
        self.commandType = None
        self.deviceType = None
        self.manufacturerId = None
        self.model = None
        self.serviceId = None
        self.method = None
        self.maxRetransmit = None

        self.index = None
        self.nodeId = None
        self.deviceId = None
        self.commandId = None

        self.type = None
        self.deviceList = None
        self.deviceLocation = None
        self.groupList = None
        self.callbackUrl = None
        self.groupTag = None

    def getFileId(self):
        return self.fileId

    def setFileId(self, fileId):
        self.fileId = fileId

    def getCommandType(self):
        return self.commandType

    def setCommandType(self, commandType):
        self.commandType = commandType

    def getDeviceType(self):
        return self.deviceType

    def setDeviceType(self, deviceType):
        self.deviceType = deviceType

    def getManufacturerId(self):
        return self.manufacturerId

    def setManufacturerId(self, manufacturerId):
        self.manufacturerId = manufacturerId

    def getModel(self):
        return self.model

    def setModel(self, model):
        self.model = model

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getMethod(self):
        return self.method

    def setMethod(self, method):
        self.method = method

    def getMaxRetransmit(self):
        return self.maxRetransmit

    def setMaxRetransmit(self, maxRetransmit):
        self.maxRetransmit = maxRetransmit

    def getIndex(self):
        return self.index

    def setIndex(self, index):
        self.index = index

    def getNodeId(self):
        return self.nodeId

    def setNodeId(self, nodeId):
        self.nodeId = nodeId

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getCommandId(self):
        return self.commandId

    def setCommandId(self, commandId):
        self.commandId = commandId

    def getType(self):
        return self.type

    def setType(self, type):
        self.type = type

    def getDeviceList(self):
        return self.deviceList

    def setDeviceList(self, deviceList):
        self.deviceList = deviceList

    def getDeviceLocation(self):
        return self.deviceLocation

    def setDeviceLocation(self, deviceLocation):
        self.deviceLocation = deviceLocation

    def getGroupList(self):
        return self.groupList

    def setGroupList(self, groupList):
        self.groupList = groupList

    def getCommand(self):
        return self.command

    def setCommand(self, command):
        self.command = command

    def getCallbackUrl(self):
        return self.callbackUrl

    def setCallbackUrl(self, callbackUrl):
        self.callbackUrl = callbackUrl

    def getGroupTag(self):
        return self.groupTag

    def setGroupTag(self, groupTag):
        self.groupTag = groupTag
