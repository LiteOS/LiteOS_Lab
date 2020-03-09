class RuleResponseBodyDTO(object):

    def __init__(self):
        self.ruleId = None
        self.response = None

    def getRuleId(self):
        return self.ruleId

    def setRuleId(self, ruleId):
        self.ruleId = ruleId

    def getResponse(self):
        return self.response

    def setResponse(self, response):
        self.response = response
