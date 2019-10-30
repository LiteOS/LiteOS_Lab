from com.huawei.iotplatform.client.dto.ServiceCommand import ServiceCommand
from com.huawei.iotplatform.client.dto.ServiceProperty import ServiceProperty


class ServiceCapabilityDTO(object):
    commands = ServiceCommand()
    properties = ServiceProperty()

    def __init__(self):
        self.serviceId = None
        self.serviceType = None
        self.option = None
        self.description = None

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getServiceType(self):
        return self.serviceType

    def setServiceType(self, serviceType):
        self.serviceType = serviceType

    def getOption(self):
        return self.option

    def setOption(self, option):
        self.option = option

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getCommands(self):
        return self.commands

    def setCommands(self, commands):
        self.commands = commands

    def getProperties(self):
        return self.properties

    def setProperties(self, properties):
        self.properties = properties
