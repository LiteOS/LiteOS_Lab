import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class DeviceUpgrade(object):
    log = Log()
    log.setLogConfig()

    def queryUpgradePackageList(self, quplInDTO, accessToken):
        quplInDTO = DictUtil.dto2dict(quplInDTO)
        authUrl = RestConstant.QUERY_UPGRADE_PACKAGE_LIST
        for key in quplInDTO.keys():
            if quplInDTO[key] != None:
                authUrl += "&" + key + "=" + quplInDTO[key]
        logging.info(quplInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryUpgradePackage(self, fileId, accessToken):
        authUrl = RestConstant.QUERY_UPGRADE_PACKAGE + fileId
        logging.info(fileId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def deleteUpgradePackage(self, fileId, accessToken):
        authUrl = RestConstant.DELETE_UPGRADE_PACKAGE + fileId
        logging.info(fileId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, None, accessToken)

    def createSoftwareUpgradeTask(self, cutInDTO, accessToken):
        cutInDTO = DictUtil.dto2dict(cutInDTO)
        authUrl = RestConstant.CREATE_SOFTWARE_UPGRADE_TASK
        payload = json.dumps(cutInDTO)
        logging.info(cutInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def createFirmwareUpgradeTask(self, cutInDTO, accessToken):
        cutInDTO = DictUtil.dto2dict(cutInDTO)
        authUrl = RestConstant.CREATE_FIRMWARE_UPGRADE_TASK
        payload = json.dumps(cutInDTO)
        logging.info(cutInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def queryUpgradeTask(self, operationId, accessToken):
        authUrl = RestConstant.QUERY_UPGRADE_TASK + operationId
        logging.info(operationId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryUpgradeSubTask(self, qustInDTO, operationId, accessToken):
        qustInDTO = DictUtil.dto2dict(qustInDTO)
        authUrl = RestConstant.QUERY_UPGRADE_SUB_TASK + operationId + "/subOperations?"
        for key in qustInDTO.keys():
            if qustInDTO[key] != None:
                authUrl += "&" + key + "=" + qustInDTO[key]
        logging.info(qustInDTO), logging.info(operationId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryUpgradeTaskList(self, qutlInDTO, accessToken):
        qutlInDTO = DictUtil.dto2dict(qutlInDTO)
        authUrl = RestConstant.QUERY_UPGRADE_TASK_LIST
        for key in qutlInDTO.keys():
            if qutlInDTO[key] != None:
                authUrl += "&" + key + "=" + qutlInDTO[key]
        logging.info(qutlInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    # def queryUpgradePackageList(self, clientInfo, quplInDTO, accessToken):
    #     quplInDTO = DictUtil.dto2dict(quplInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_UPGRADE_PACKAGE_LIST
    #     for key in quplInDTO.keys():
    #         if quplInDTO[key] != None:
    #             authUrl += "&" + key + "=" + quplInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryUpgradePackage(self, clientInfo, fileId,  accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_UPGRADE_PACKAGE + fileId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def deleteUpgradePackage(self, clientInfo, fileId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.DELETE_UPGRADE_PACKAGE + fileId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, headers, None)
    #
    # def createSoftwareUpgradeTask(self, clientInfo, cutInDTO, accessToken):
    #     cutInDTO = DictUtil.dto2dict(cutInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_SOFTWARE_UPGRADE_TASK
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(cutInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def createFirmwareUpgradeTask(self, clientInfo, cutInDTO, accessToken):
    #     cutInDTO = DictUtil.dto2dict(cutInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_FIRMWARE_UPGRADE_TASK
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(cutInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def queryUpgradeTask(self, clientInfo, operationId,  accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_UPGRADE_TASK + operationId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryUpgradeSubTask(self, clientInfo, qustInDTO, operationId,  accessToken):
    #     qustInDTO = DictUtil.dto2dict(qustInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_UPGRADE_SUB_TASK + operationId + "/subOperations?"
    #     for key in qustInDTO.keys():
    #         if qustInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qustInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryUpgradeTaskList(self, clientInfo, qutlInDTO, accessToken):
    #     qutlInDTO = DictUtil.dto2dict(qutlInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_UPGRADE_TASK_LIST
    #     for key in qutlInDTO.keys():
    #         if qutlInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qutlInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
