from com.huawei.iotplatform.client.dto.ServiceDesiredDTO import ServiceDesiredDTO


class ModifyDeviceShadowInDTO(object):
    serviceDesireds = ServiceDesiredDTO

    def __init__(self):
        pass

    def getServiceDesireds(self):
        return self.serviceDesireds

    def setServiceDesireds(self, serviceDesireds):
        self.serviceDesireds = serviceDesireds
