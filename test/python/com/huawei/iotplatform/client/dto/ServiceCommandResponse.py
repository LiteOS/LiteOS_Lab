from com.huawei.iotplatform.client.dto.ServiceCommandPara import ServiceCommandPara


class ServiceCommandResponse(object):
    paras = ServiceCommandPara()

    def __init__(self):
        self.responseName = None

    def getResponseName(self):
        return self.responseName

    def setResponseName(self, responseName):
        self.responseName = responseName

    def getParas(self):
        return self.paras

    def setParas(self, paras):
        self.paras = paras
