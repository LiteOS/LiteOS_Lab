from com.huawei.iotplatform.client.dto.ActionResult import ActionResult
from com.huawei.iotplatform.client.dto.ConditionReason import ConditionReason


class NotifyRuleEventDTO(object):
    reasons = ConditionReason()
    actionsResults = ActionResult()

    def __init__(self):
        self.notifyType = None
        self.author = None
        self.ruleId = None
        self.ruleName = None
        self.logic = None
        self.triggerTime = None

    def getNotifyType(self):
        return self.notifyType

    def setNotifyType(self, notifyType):
        self.notifyType = notifyType

    def getAuthor(self):
        return self.author

    def setAuthor(self, author):
        self.author = author

    def getRuleId(self):
        return self.ruleId

    def setRuleId(self, ruleId):
        self.ruleId = ruleId

    def getRuleName(self):
        return self.ruleName

    def setRuleName(self, ruleName):
        self.ruleName = ruleName

    def getLogic(self):
        return self.logic

    def setLogic(self, logic):
        self.logic = logic

    def getTriggerTime(self):
        return self.triggerTime

    def setTriggerTime(self, triggerTime):
        self.triggerTime = triggerTime

    def getReasons(self):
        return self.reasons

    def setReasons(self, reasons):
        self.reasons = reasons

    def getActionsResults(self):
        return self.actionsResults

    def setActionsResults(self, actionsResults):
        self.actionsResults = actionsResults
