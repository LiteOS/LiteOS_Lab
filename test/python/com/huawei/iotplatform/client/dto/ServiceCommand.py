from com.huawei.iotplatform.client.dto.ServiceCommandPara import ServiceCommandPara
from com.huawei.iotplatform.client.dto.ServiceCommandResponse import ServiceCommandResponse


class ServiceCommand(object):
    paras = ServiceCommandPara()
    responses = ServiceCommandResponse()

    def __init__(self):
        self.commandName = None

    def getCommandName(self):
        return self.commandName

    def setCommandName(self, commandName):
        self.commandName = commandName

    def getParas(self):
        return self.paras

    def setParas(self, paras):
        self.paras = paras

    def getResponses(self):
        return self.responses

    def setResponses(self, responses):
        self.responses = responses
