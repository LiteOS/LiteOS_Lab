import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class DeviceGroupManagement(object):
    log = Log()
    log.setLogConfig()

    def createDeviceGroup(self, cdgInDTO, accessToken):
        cdgInDTO = DictUtil.dto2dict(cdgInDTO)
        authUrl = RestConstant.CREATE_DEVICE_GROUP
        payload = json.dumps(cdgInDTO)
        logging.info(cdgInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def deleteDeviceGroup(self, devGroupId, accessAppId, accessToken):
        authUrl = RestConstant.DELETE_DEVICE_GROUP + devGroupId
        if accessAppId != None:
            authUrl += "?accessAppId=" + accessAppId
        logging.info(devGroupId), logging.info(accessAppId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, None, accessToken)

    def modifyDeviceGroup(self, mdgInDTO, devGroupId, accessAppId, accessToken):
        mdgInDTO = DictUtil.dto2dict(mdgInDTO)
        authUrl = RestConstant.MODIFY_DEVICE_GROUP + devGroupId
        if accessAppId != None:
            authUrl += "?accessAppId=" + accessAppId
        payload = json.dumps(mdgInDTO)
        logging.info(mdgInDTO), logging.info(devGroupId), logging.info(accessAppId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    def queryDeviceGroups(self, qdgInDTO, accessToken):
        qdgInDTO = DictUtil.dto2dict(qdgInDTO)
        authUrl = RestConstant.QUERY_DEVICE_GROUPS
        for key in qdgInDTO.keys():
            if qdgInDTO[key] != None:
                authUrl += "&" + key + "=" + qdgInDTO[key]
        logging.info(qdgInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def querySingleDeviceGroup(self, devGroupId, accessAppId, accessToken):
        authUrl = RestConstant.QUERY_SINGLE_DEVICE_GROUP + devGroupId
        if accessAppId != None:
            authUrl += "?accessAppId=" + accessAppId
        logging.info(devGroupId), logging.info(accessAppId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryDeviceGroupMembers(self, qdgmInDTO, accessToken):
        qdgmInDTO = DictUtil.dto2dict(qdgmInDTO)
        authUrl = RestConstant.QUERY_DEVICE_GROUP_MEMBERS + qdgmInDTO['devGroupId']
        for key in qdgmInDTO.keys():
            if key == 'devGroupId':
                authUrl = authUrl
            elif qdgmInDTO[key] != None:
                authUrl += "&" + key + "=" + qdgmInDTO[key]
        logging.info(qdgmInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def addDevicesToGroup(self, dgwdlDTO, accessAppId, accessToken):
        dgwdlDTO = DictUtil.dto2dict(dgwdlDTO)
        authUrl = RestConstant.ADD_DEVICES_TO_GROUP
        if accessAppId != None:
            authUrl += "?accessAppId=" + accessAppId
        payload = json.dumps(dgwdlDTO)
        logging.info(dgwdlDTO), logging.info(accessAppId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def deleteDevicesFromGroup(self, dgwdlDTO, accessAppId, accessToken):
        dgwdlDTO = DictUtil.dto2dict(dgwdlDTO)
        authUrl = RestConstant.DELETE_DEVICES_FROM_GROUP
        if accessAppId != None:
            authUrl += "?accessAppId=" + accessAppId
        payload = json.dumps(dgwdlDTO)
        logging.info(dgwdlDTO), logging.info(accessAppId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    # def createDeviceGroup(self, clientInfo, cdgInDTO, accessToken):
    #     cdgInDTO = DictUtil.dto2dict(cdgInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.CREATE_DEVICE_GROUP
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(cdgInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def deleteDeviceGroup(self, clientInfo, devGroupId, accessAppId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.DELETE_DEVICE_GROUP + devGroupId
    #     if accessAppId != None:
    #         authUrl += "?accessAppId=" + accessAppId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, headers, None)
    #
    # def modifyDeviceGroup(self, clientInfo, mdgInDTO, devGroupId, accessAppId, accessToken):
    #     mdgInDTO = DictUtil.dto2dict(mdgInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.MODIFY_DEVICE_GROUP + devGroupId
    #     if accessAppId != None:
    #         authUrl += "?accessAppId=" + accessAppId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(mdgInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
    #
    # def queryDeviceGroups(self, clientInfo, qdgInDTO, accessToken):
    #     qdgInDTO = DictUtil.dto2dict(qdgInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_GROUPS
    #     for key in qdgInDTO.keys():
    #         if qdgInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qdgInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def querySingleDeviceGroup(self, clientInfo, devGroupId, accessAppId, accessToken):
    #     # qsdgInDTO = DictUtil.dto2dict(qsdgInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_SINGLE_DEVICE_GROUP + devGroupId
    #     if accessAppId != None:
    #         authUrl += "?accessAppId=" + accessAppId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryDeviceGroupMembers(self, clientInfo, qdgmInDTO, accessToken):
    #     qdgmInDTO = DictUtil.dto2dict(qdgmInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_GROUP_MEMBERS + qdgmInDTO['devGroupId']
    #     for key in qdgmInDTO.keys():
    #         if key == 'devGroupId':
    #             authUrl = authUrl
    #         elif qdgmInDTO[key] != None:
    #             authUrl += "&" + key + "=" + qdgmInDTO[key]
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def addDevicesToGroup(self, clientInfo, dgwdlDTO, accessAppId, accessToken):
    #     dgwdlDTO = DictUtil.dto2dict(dgwdlDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.ADD_DEVICES_TO_GROUP
    #     if accessAppId != None:
    #         authUrl += "?accessAppId=" + accessAppId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(dgwdlDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def deleteDevicesFromGroup(self, clientInfo, dgwdlDTO, accessAppId, accessToken):
    #     dgwdlDTO = DictUtil.dto2dict(dgwdlDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.DELETE_DEVICES_FROM_GROUP
    #     if accessAppId != None:
    #         authUrl += "?accessAppId=" + accessAppId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(dgwdlDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
