from com.huawei.iotplatform.client.dto.CommandNA2CloudHeader import CommandNA2CloudHeader


class CommandDTO(object):
    header = CommandNA2CloudHeader()
    body = None

    def __init__(self):
        pass

    def getBody(self):
        return self.body

    def setBody(self, body):
        self.body = body

    def getHeader(self):
        return self.header

    def setHeader(self, header):
        self.header = header
