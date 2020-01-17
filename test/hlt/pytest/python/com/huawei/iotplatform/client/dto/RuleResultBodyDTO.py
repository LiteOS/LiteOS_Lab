class RuleResultBodyDTO(object):

    def __init__(self):
        self.result_code = str()
        self.result_desc = str()

    def getResult_code(self):
        return self.result_code

    def setResult_code(self, result_code):
        self.result_code = result_code

    def getResult_desc(self):
        return self.result_desc

    def setResult_desc(self, result_desc):
        self.result_desc = result_desc
