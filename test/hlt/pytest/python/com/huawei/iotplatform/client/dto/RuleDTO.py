class RuleDTO(object):

    def __init__(self):
        self.ruleId = None
        self.appKey = None
        self.name = None
        self.description = None
        self.author = None
        self.conditions = None
        self.logic = None
        self.timeRange = None
        self.actions = None
        self.matchNow = None
        self.status = None
        self.groupExpress = None
        self.triggerSources = None
        self.timezoneID = None
        self.transData = None
        self.executor = None
        self.refreshId = None
        self.checkNullAction = None
        self.priority = None
        self.tags = None
        self.rulePreProcessors = None

    def getRuleId(self):
        return self.ruleId

    def setRuleId(self, ruleId):
        self.ruleId = ruleId

    def getAppKey(self):
        return self.appKey

    def setAppKey(self, appKey):
        self.appKey = appKey

    def getName(self):
        return self.name

    def setName(self, name):
        self.name = name

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getAuthor(self):
        return self.author

    def setAuthor(self, author):
        self.author = author

    def getConditions(self):
        return self.conditions

    def setConditions(self, conditions):
        self.conditions = conditions

    def getLogic(self):
        return self.logic

    def setLogic(self, logic):
        self.logic = logic

    def getTimeRange(self):
        return self.timeRange

    def setTimeRange(self, timeRange):
        self.timeRange = timeRange

    def getActions(self):
        return self.actions

    def setActions(self, actions):
        self.actions = actions

    def getMatchNow(self):
        return self.matchNow

    def setMatchNow(self, matchNow):
        self.matchNow = matchNow

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getGroupExpress(self):
        return self.groupExpress

    def setGroupExpress(self, groupExpress):
        self.groupExpress = groupExpress

    def getTriggerSources(self):
        return self.triggerSources

    def setTriggerSources(self, triggerSources):
        self.triggerSources = triggerSources

    def getTimezoneID(self):
        return self.timezoneID

    def setTimezoneID(self, timezoneID):
        self.timezoneID = timezoneID

    def getTransData(self):
        return self.transData

    def setTransData(self, transData):
        self.transData = transData

    def getExecutor(self):
        return self.executor

    def setExecutor(self, executor):
        self.executor = executor

    def getRefreshId(self):
        return self.refreshId

    def setRefreshId(self, refreshId):
        self.refreshId = refreshId

    def getCheckNullAction(self):
        return self.checkNullAction

    def setCheckNullAction(self, checkNullAction):
        self.checkNullAction = checkNullAction

    def getPriority(self):
        return self.priority

    def setPriority(self, priority):
        self.priority = priority
