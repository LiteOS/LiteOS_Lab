import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class BatchProcess(object):
    log = Log()
    log.setLogConfig()

    def createBatchTask(self, btcInDTO, accessToken):
        btcInDTO = DictUtil.dto2dict(btcInDTO)
        authUrl = RestConstant.CREATE_BATCH_TASK
        payload = json.dumps(btcInDTO)
        logging.info(btcInDTO), logging.info(accessToken), logging.info(payload)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def queryOneTask(self, taskId, select, appId, accessToken):
        authUrl = RestConstant.CREATE_BATCH_TASK + taskId + "?"
        if appId != None:
            authUrl += "appId=" + appId
        if select != None:
            authUrl += "&select=" + select
        logging.info(taskId), logging.info(select), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryTaskDetails(self, qtdInDTO, accessToken):
        qtdInDTO = DictUtil.dto2dict(qtdInDTO)
        authUrl = RestConstant.QUERYTASKDETAILS + str(qtdInDTO['taskId'])
        for key in qtdInDTO.keys():
            if key == 'taskId':
                authUrl = authUrl
            elif qtdInDTO[key] != None:
                authUrl += "&" + key + "=" + qtdInDTO[key]
        logging.info(qtdInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    # def createBatchTask(self, clientInfo, btcInDTO, accessToken):
    #     btcInDTO = DictUtil.dto2dict(btcInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_BATCH_TASK
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(btcInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def queryOneTask(self, clientInfo, taskId, select, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_BATCH_TASK + taskId + "?"
    #     if appId != None:
    #         authUrl += "appId=" + appId
    #     if select != None:
    #         authUrl += "&select=" + select
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryTaskDetails(self, clientInfo, qtdInDTO, accessToken):
    #     qtdInDTO = DictUtil.dto2dict(qtdInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERYTASKDETAILS + qtdInDTO['taskId']
    #     for key in qtdInDTO.keys():
    #         if key == 'taskId':
    #             authUrl = authUrl
    #         elif qtdInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qtdInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
