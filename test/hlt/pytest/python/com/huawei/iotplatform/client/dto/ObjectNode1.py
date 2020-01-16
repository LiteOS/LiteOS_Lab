class ObjectNode1(object):

    def __init__(self):
        self.index = None
        self.nodeId = None
        self.deviceId = None
        self.commandId = None

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
