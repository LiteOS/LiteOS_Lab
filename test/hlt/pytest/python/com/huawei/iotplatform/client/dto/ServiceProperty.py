class ServiceProperty(object):

    def __init__(self):
        self.propertyName = None
        self.dataType = None
        self.required = None
        self.min = None
        self.max = None
        self.step = None
        self.maxLength = None
        self.method = None
        self.unit = None
        self.enumList = None

    def getPropertyName(self):
        return self.propertyName

    def setPropertyName(self, propertyName):
        self.propertyName = propertyName

    def getDataType(self):
        return self.dataType

    def setDataType(self, dataType):
        self.dataType = dataType

    def isRequired(self):
        return self.required

    def setRequired(self, required):
        self.required = required

    def getMin(self):
        return self.min

    def setMin(self, min):
        self.min = min

    def getMax(self):
        return self.max

    def setMax(self, max):
        self.max = max

    def getStep(self):
        return self.step

    def setStep(self, step):
        self.step = step

    def getMaxLength(self):
        return self.maxLength

    def setMaxLength(self, maxLength):
        self.maxLength = maxLength

    def getMethod(self):
        return self.method

    def setMethod(self, method):
        self.method = method

    def getUnit(self):
        return self.unit

    def setUnit(self, unit):
        self.unit = unit

    def getEnumList(self):
        return self.enumList

    def setEnumList(self, enumList):
        self.enumList = enumList
