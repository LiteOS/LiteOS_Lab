from com.huawei.iotplatform.client.dto.ObjectNode1 import ObjectNode1


class QueryTaskDetailDTOCloud2NA(object):
    param = ObjectNode1()

    def __init__(self):
        self.status = None
        self.output = None
        self.error = None

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getOutput(self):
        return self.output

    def setOutput(self, output):
        self.output = output

    def getError(self):
        return self.error

    def setError(self, error):
        self.error = error

    def getParam(self):
        return self.param

    def setParam(self, param):
        self.param = param
