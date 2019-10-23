class DeleteDeviceGroupInDTO(object):

    def __init__(self):
        self.devGroupId = None
        self.accessAppId = None

    def getDevGroupId(self):
        return self.devGroupId

    def setDevGroupId(self, devGroupId):
        self.devGroupId = devGroupId

    def getAccessAppId(self):
        return self.accessAppId

    def setAccessAppId(self, accessAppId):
        self.accessAppId = accessAppId
