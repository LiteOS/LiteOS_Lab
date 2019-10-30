import json
import logging.config
from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class Authentication(object):
    log = Log()
    log.setLogConfig()

    def getAuthToken(self, clientInfo):
        authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
            'platformPort'] + RestConstant.APP_AUTH
        headers = {'Content-Type': 'application/x-www-form-urlencoded'}
        payload = {
            'appId': clientInfo['appId'],
            'secret': clientInfo['secret']
        }
        logging.info(clientInfo), logging.info(authUrl), logging.info(headers), logging.info(payload)
        return NorthApiClient.invokeAPI2(RestConstant.HTTPPOST, authUrl, headers, payload)

    def refreshAuthToken(self, arInDTO):
        arInDTO = DictUtil.dto2dict(arInDTO)
        authUrl = RestConstant.REFRESH_TOKEN
        payload = json.dumps(arInDTO)
        logging.info(arInDTO), logging.info(payload)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, None)

    # def getAuthToken(self, clientInfo):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.APP_AUTH
    #     headers = {'Content-Type': 'application/x-www-form-urlencoded'}
    #     payload = {
    #         'appId': clientInfo['appId'],
    #         'secret': clientInfo['secret']
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def refreshAuthToken(self, clientInfo, arInDTO):
    #     arInDTO = DictUtil.dto2dict(arInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.REFRESH_TOKEN
    #     headers = {'Content-Type': 'application/json'}
    #     payload = json.dumps(arInDTO)
    #     return NorthApiClient.invokeAPI2(RestConstant.HTTPPOST, authUrl, headers, payload)
