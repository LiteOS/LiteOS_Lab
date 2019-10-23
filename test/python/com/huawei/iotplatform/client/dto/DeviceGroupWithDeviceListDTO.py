class DeviceGroupWithDeviceListDTO(object):

    def __init__(self):
        self.devGroupId = None
        self.deviceIds = None
        self.accessAppId = None

    def getDevGroupId(self):
        return self.devGroupId

    def setDevGroupId(self, devGroupId):
        self.devGroupId = devGroupId

    def getDeviceIds(self):
        return self.deviceIds

    def setDeviceIds(self, deviceIds):
        self.deviceIds = deviceIds

    def getAccessAppId(self):
        return self.accessAppId

    def setAccessAppId(self, accessAppId):
        self.accessAppId = accessAppId
