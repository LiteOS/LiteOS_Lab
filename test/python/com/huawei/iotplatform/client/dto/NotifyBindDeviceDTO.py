from com.huawei.iotplatform.client.dto.DeviceInfo import DeviceInfo


class NotifyBindDeviceDTO(object):
    deviceInfo = DeviceInfo()

    def __init__(self):
        self.notifyType = None
        self.deviceId = None
        self.resultCode = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getDeviceId(self):
        return self.deviceId

    def setDeviceId(self, deviceId):
        self.deviceId = deviceId

    def getResultCode(self):
        return self.resultCode

    def setResultCode(self, resultCode):
        self.resultCode = resultCode
