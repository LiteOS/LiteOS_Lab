class ServiceDesiredDTO(object):
    serviceId = "serviceId"
    desired = "desired"

    def __init__(self):
        pass

    def getServiceId(self):
        return self.serviceId

    def setServiceId(self, serviceId):
        self.serviceId = serviceId

    def getDesired(self):
        return self.desired

    def setDesired(self, desired):
        self.desired = desired
