import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class RuleEngine(object):
    log = Log()
    log.setLogConfig()

    def createRule(self, ruleDTO, appId, accessToken):
        ruleDTO = DictUtil.dto2dict(ruleDTO)
        authUrl = RestConstant.CREATE_RULE
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(ruleDTO)
        logging.info(ruleDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def updateRule(self, ruleDTO, appId, accessToken):
        ruleDTO = DictUtil.dto2dict(ruleDTO)
        authUrl = RestConstant.UPDATE_RULE
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(ruleDTO)
        logging.info(ruleDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    def deleteRule(self, ruleId, appId, accessToken):
        authUrl = RestConstant.DELETE_RULE + ruleId
        if appId != None:
            authUrl += "?appId=" + appId
        logging.info(ruleId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, None, accessToken)

    def queryRules(self, qrInDTO, accessToken):
        qrInDTO = DictUtil.dto2dict(qrInDTO)
        authUrl = RestConstant.QUERY_RULES + qrInDTO['author']
        for key in qrInDTO.keys():
            if key == 'author':
                authUrl = authUrl
            elif qrInDTO[key] != None:
                authUrl += "&" + key + "=" + qrInDTO[key]
        logging.info(qrInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def updateRuleStatus(self, ursInDTO, appId, accessToken):
        ursInDTO = DictUtil.dto2dict(ursInDTO)
        authUrl = RestConstant.UPDATE_RULE_STATUS + ursInDTO['ruleId'] + "/status/" + ursInDTO['status']
        if appId != None:
            authUrl += "?appId=" + appId
        logging.info(ursInDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, None, accessToken)

    def updateBatchRuleStatus(self, ubrsInDTO, appId, accessToken):
        ubrsInDTO = DictUtil.dto2dict(ubrsInDTO)
        authUrl = RestConstant.UPDATE_BATCH_RULE_STATUS
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(ubrsInDTO)
        logging.info(ubrsInDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    # def createRule(self, clientInfo, ruleDTO, appId, accessToken):
    #     ruleDTO = DictUtil.dto2dict(ruleDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_RULE
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(ruleDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def updateRule(self, clientInfo, ruleDTO, appId, accessToken):
    #     ruleDTO = DictUtil.dto2dict(ruleDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.UPDATE_RULE
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(ruleDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
    #
    # def deleteRule(self, clientInfo, ruleId, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.DELETE_RULE + ruleId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, headers, None)
    #
    # def queryRules(self, clientInfo, qrInDTO, accessToken):
    #     qrInDTO = DictUtil.dto2dict(qrInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_RULES + qrInDTO['author']
    #     for key in qrInDTO.keys():
    #         if key == 'author':
    #             authUrl = authUrl
    #         elif qrInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qrInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def updateRuleStatus(self, clientInfo, ursInDTO, appId, accessToken):
    #     ursInDTO = DictUtil.dto2dict(ursInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.UPDATE_RULE_STATUS + ursInDTO['ruleId'] + "/status/" + ursInDTO['status']
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, None)
    #
    # def updateBatchRuleStatus(self, clientInfo, ubrsInDTO, appId, accessToken):
    #     ubrsInDTO = DictUtil.dto2dict(ubrsInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.UPDATE_BATCH_RULE_STATUS
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(ubrsInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
