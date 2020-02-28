class ModifyDeviceGroupInDTO(object):

    def __init__(self):
        self.name = None
        self.description = None
        self.maxDevNum = None

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getMaxDevNum(self):
        return self.maxDevNum

    def setMaxDevNum(self, maxDevNum):
        self.maxDevNum = maxDevNum
