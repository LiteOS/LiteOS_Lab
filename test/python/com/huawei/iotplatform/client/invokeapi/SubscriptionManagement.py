import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class SubscriptionManagement(object):
    log = Log()
    log.setLogConfig()

    def subDeviceBusinessData(self, sdbdInDTO, accessToken):
        sdbdInDTO = DictUtil.dto2dict(sdbdInDTO)
        authUrl = RestConstant.SUB_DEVICE_BUSINESS_DATA
        if sdbdInDTO['ownerFlag'] != None:
            authUrl += "?ownerFlag=" + sdbdInDTO['ownerFlag']
        payload = json.dumps(sdbdInDTO)
        logging.info(sdbdInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def subDeviceManagementData(self, sdmdInDTO, accessToken):
        sdmdInDTO = DictUtil.dto2dict(sdmdInDTO)
        authUrl = RestConstant.SUB_DEVICE_MANAGEMENT_DATA
        payload = json.dumps(sdmdInDTO)
        logging.info(sdmdInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def querySingleSubscription(self, subscriptionId, appId, accessToken):
        authUrl = RestConstant.QUERY_SINGLE_SUB_SCRIPTION + subscriptionId
        if appId != None:
            authUrl += "?appId=" + appId
        logging.info(subscriptionId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryBatchSubscriptions(self, qbsInDTO, accessToken):
        qbsInDTO = DictUtil.dto2dict(qbsInDTO)
        authUrl = RestConstant.QUERY_BATCH_SUB_SCRIPTIONS
        for key in qbsInDTO.keys():
            if qbsInDTO[key] != None:
                authUrl += "&" + key + "=" + qbsInDTO[key]
        logging.info(qbsInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def deleteSingleSubscription(self, subscriptionId, appId, accessToken):
        authUrl = RestConstant.DELETE_SINGLE_SUB_SCRIPTION + subscriptionId
        if appId != None:
            authUrl += "?appId=" + appId
        logging.info(subscriptionId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, None, accessToken)

    def deleteBatchSubscriptions(self, dbsInDTO, accessToken):
        dbsInDTO = DictUtil.dto2dict(dbsInDTO)
        authUrl = RestConstant.DELETE_BATCH_SUB_SCRIPTIONS
        for key in dbsInDTO.keys():
            if dbsInDTO[key] != None:
                authUrl += "&" + key + "=" + dbsInDTO[key]
        logging.info(dbsInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, None, accessToken)

    # def subDeviceBusinessData(self, clientInfo, sdbdInDTO, accessToken):
    #     sdbdInDTO = DictUtil.dto2dict(sdbdInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.SUB_DEVICE_BUSINESS_DATA
    #     if sdbdInDTO['ownerFlag'] != None:
    #         authUrl += "?ownerFlag=" + sdbdInDTO['ownerFlag']
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(sdbdInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def subDeviceManagementData(self, clientInfo, sdmdInDTO, accessToken):
    #     sdmdInDTO = DictUtil.dto2dict(sdmdInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.SUB_DEVICE_MANAGEMENT_DATA
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(sdmdInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def querySingleSubscription(self, clientInfo, subscriptionId, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_SINGLE_SUB_SCRIPTION + subscriptionId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryBatchSubscriptions(self, clientInfo, qbsInDTO, accessToken):
    #     qbsInDTO = DictUtil.dto2dict(qbsInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_BATCH_SUB_SCRIPTIONS
    #     for key in qbsInDTO.keys():
    #         if qbsInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qbsInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def deleteSingleSubscription(self, clientInfo, subscriptionId, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.DELETE_SINGLE_SUB_SCRIPTION + subscriptionId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, headers, None)
    #
    # def deleteBatchSubscriptions(self, clientInfo, dbsInDTO, accessToken):
    #     dbsInDTO = DictUtil.dto2dict(dbsInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.DELETE_BATCH_SUB_SCRIPTIONS
    #     for key in dbsInDTO.keys():
    #         if dbsInDTO[key] != None:
    #             authUrl += "&" + key + "=" + dbsInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, headers, None)
