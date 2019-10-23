class CustomField(object):

    def __init__(self):
        self.fieldName = None
        self.fieldType = None
        self.fieldValue = None

    def getFieldName(self):
        return self.fieldName

    def setFieldName(self, fieldName):
        self.fieldName = fieldName

    def getFieldType(self):
        return self.fieldType

    def setFieldType(self, fieldType):
        self.fieldType = fieldType

    def getFieldValue(self):
        return self.fieldValue

    def setFieldValue(self, fieldValue):
        self.fieldValue = fieldValue
