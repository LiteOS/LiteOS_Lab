import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.RuleEngine import RuleEngine

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.QueryRulesInDTO import QueryRulesInDTO
from com.huawei.iotplatform.client.dto.RuleCreateOrUpdateOutDTO import RuleCreateOrUpdateOutDTO
from com.huawei.iotplatform.client.dto.RuleDTO import RuleDTO
from com.huawei.iotplatform.client.dto.UpdateBatchRuleStatusInDTO import UpdateBatchRuleStatusInDTO
from com.huawei.iotplatform.client.dto.UpdateRuleStatusInDTO import UpdateRuleStatusInDTO
from com.huawei.iotplatform.constant.Constant import Constant


class RuleEngineTest(object):
    def createRuleInfo(self):
        ruleDTO = RuleDTO()
        ruleDTO.name = "a1"
        ruleDTO.author = "ass"
        return ruleDTO

    def updateRuleInfo(self):
        ruleDTO = RuleDTO()
        ruleDTO.ruleId = "sss"
        ruleDTO.name = "a"
        ruleDTO.author = "ass"
        return ruleDTO

    def updateRuleStatusInfo(self):
        ursInDTO = UpdateRuleStatusInDTO()
        ursInDTO.ruleId = "sasa"
        ursInDTO.status = "inactive"
        return ursInDTO

    def queryRulesInfo(self):
        qrInDTO = QueryRulesInDTO()
        qrInDTO.author = "a111112"
        qrInDTO.name = "111"
        qrInDTO.select = "tag"
        return qrInDTO

    def updateBatchRuleStatusInfo(self):
        ubrsInDTO = UpdateBatchRuleStatusInDTO()
        ubrsInDTO.requests = [
            {ubrsInDTO.ruleId: "as",
             ubrsInDTO.status: "active"},
            {ubrsInDTO.ruleId: "ss",
             ubrsInDTO.status: "active"}]
        return ubrsInDTO


if __name__ == "__main__":
    reTest = RuleEngineTest()
    authentication = Authentication()
    ruleEngine = RuleEngine()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # create a rule
    rc = ruleEngine.createRule(reTest.createRuleInfo(), None, accessToken)
    print("====== create a rule ======")
    print("result:", rc + "\n")

    # get ruleId
    rcouOutDTO = RuleCreateOrUpdateOutDTO()
    rcouOutDTO.setRuleId(json.loads(rc)['ruleId'])
    ruleId = rcouOutDTO.getRuleId()
    print("ruleId==", ruleId)

    # update the rule
    rc = ruleEngine.updateRule(reTest.updateRuleInfo(), None, accessToken)
    print("====== update the rule ======")
    print("result:", rc + "\n")

    # update rule status
    ru = ruleEngine.updateRuleStatus(reTest.updateRuleStatusInfo(), None, accessToken)
    print("====== update rule status ======")
    print("result:", ru)
    print("--------------------------------")

    # query rules
    rq = ruleEngine.queryRules(reTest.queryRulesInfo(), accessToken)
    print("====== query rules ======")
    print("result:", rq + "\n")

    # batch update rule status
    ru = ruleEngine.updateBatchRuleStatus(reTest.updateBatchRuleStatusInfo(), None, accessToken)
    print("====== batch update rule status ======")
    print("result:", ru + "\n")

    # delete rule
    ruleId = "qq"
    rd = ruleEngine.deleteRule(ruleId, None, accessToken)
    print("====== delete rule ======")
    print("result:", rd)
