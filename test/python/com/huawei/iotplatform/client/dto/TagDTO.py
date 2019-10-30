class TagDTO(object):

    def __init__(self):
        self.tagName = None
        self.tagValue = None

    def getTagName(self):
        return self.tagName

    def setTagName(self, tagName):
        self.tagName = tagName

    def getTagValue(self):
        return self.tagValue

    def setTagValue(self, tagValue):
        self.tagValue = tagValue
