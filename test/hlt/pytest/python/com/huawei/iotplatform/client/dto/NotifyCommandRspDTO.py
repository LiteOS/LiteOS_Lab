from com.huawei.iotplatform.client.dto.CommandRspHeader import CommandRspHeader


class NotifyCommandRspDTO(object):
    header = CommandRspHeader()

    def __init__(self):
        self.notifyType = None
        self.body = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getBody(self):
        return self.body

    def setBody(self, body):
        self.body = body

    def getHeader(self):
        return self.header

    def setHeader(self, header):
        self.header = header
