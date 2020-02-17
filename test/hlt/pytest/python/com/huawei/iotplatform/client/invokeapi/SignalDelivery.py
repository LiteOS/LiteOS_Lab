import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class SignalDelivery(object):
    log = Log()
    log.setLogConfig()

    def postDeviceCommand(self, pdcInDTO, appId, accessToken):
        pdcInDTO = DictUtil.dto2dict(pdcInDTO)
        authUrl = RestConstant.POST_DEVICE_COMMAND
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(pdcInDTO)
        logging.info(pdcInDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def queryDeviceCommand(self, qdcInDTO, accessToken):
        qdcInDTO = DictUtil.dto2dict(qdcInDTO)
        authUrl = RestConstant.QUERY_DEVICE_COMMAND
        for key in qdcInDTO.keys():
            if qdcInDTO[key] != None:
                authUrl += "&" + key + "=" + qdcInDTO[key]
        logging.info(qdcInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def updateDeviceCommand(self, udcInDTO, deviceCommandId, appId, accessToken):
        udcInDTO = DictUtil.dto2dict(udcInDTO)
        authUrl = RestConstant.UPDATE_DEVICE_COMMAND + deviceCommandId
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(udcInDTO)
        logging.info(udcInDTO), logging.info(deviceCommandId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    def createDeviceCmdCancelTask(self, cdcctInDTO, appId, accessToken):
        cdcctInDTO = DictUtil.dto2dict(cdcctInDTO)
        authUrl = RestConstant.CREATE_DEVICE_CMD_CANCEL_TASK
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(cdcctInDTO)
        logging.info(cdcctInDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def queryDeviceCmdCancelTask(self, qdcctInDTO, accessToken):
        qdcctInDTO = DictUtil.dto2dict(qdcctInDTO)
        authUrl = RestConstant.QUERY_DEVICE_CMD_CANCEL_TASK
        for key in qdcctInDTO.keys():
            if qdcctInDTO[key] != None:
                authUrl += "&" + key + "=" + qdcctInDTO[key]
        logging.info(qdcctInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    # def postDeviceCommand(self, clientInfo, pdcInDTO, appId, accessToken):
    #     pdcInDTO = DictUtil.dto2dict(pdcInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.POST_DEVICE_COMMAND
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(pdcInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def queryDeviceCommand(self, clientInfo, qdcInDTO, accessToken):
    #     qdcInDTO = DictUtil.dto2dict(qdcInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_COMMAND
    #     for key in qdcInDTO.keys():
    #         if qdcInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qdcInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def updateDeviceCommand(self, clientInfo, udcInDTO, deviceCommandId, appId, accessToken):
    #     udcInDTO = DictUtil.dto2dict(udcInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.UPDATE_DEVICE_COMMAND + deviceCommandId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(udcInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
    #
    # def createDeviceCmdCancelTask(self, clientInfo, cdcctInDTO, appId, accessToken):
    #     cdcctInDTO = DictUtil.dto2dict(cdcctInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_DEVICE_CMD_CANCEL_TASK
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(cdcctInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def queryDeviceCmdCancelTask(self, clientInfo, qdcctInDTO, accessToken):
    #     qdcctInDTO = DictUtil.dto2dict(qdcctInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_CMD_CANCEL_TASK
    #     for key in qdcctInDTO.keys():
    #         if qdcctInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qdcctInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
