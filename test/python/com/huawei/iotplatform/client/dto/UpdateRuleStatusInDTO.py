class UpdateRuleStatusInDTO(object):

    def __init__(self):
        self.ruleId = None
        self.status = None

    def getRuleId(self):
        return self.ruleId

    def setRuleId(self, ruleId):
        self.ruleId = ruleId

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status