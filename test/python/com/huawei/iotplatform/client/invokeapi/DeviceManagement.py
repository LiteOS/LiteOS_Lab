import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class DeviceManagement(object):
    log = Log()
    log.setLogConfig()

    def regDirectDevice(self, rddInDto, appId, accessToken):
        rddInDto = DictUtil.dto2dict(rddInDto)
        authUrl = RestConstant.REG_DIRECT_DEVICE
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(rddInDto)
        logging.info(rddInDto), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def refreshDeviceKey(self, rdkInDTO, appId, accessToken):
        rdkInDTO = DictUtil.dto2dict(rdkInDTO)
        authUrl = RestConstant.REFRESH_DEVICE_KEY + rdkInDTO['deviceId']
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(rdkInDTO)
        logging.info(rdkInDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    def modifyDeviceInfo(self, mdiInDto, deviceId, appId, accessToken):
        mdiInDto = DictUtil.dto2dict(mdiInDto)
        authUrl = RestConstant.MODIFY_DEVICE_INFO + deviceId
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(mdiInDto)
        logging.info(mdiInDto), logging.info(deviceId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    def deleteDirectDevice(self, deviceId, cascade, appId, accessToken):
        authUrl = RestConstant.MODIFY_DEVICE_INFO + deviceId + "?"
        if appId != None:
            authUrl += "&appId=" + appId
        if cascade != None:
            authUrl += "&cascade=" + cascade
        logging.info(deviceId), logging.info(cascade), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, None, accessToken)

    def queryDeviceStatus(self, deviceId, appId, accessToken):
        authUrl = RestConstant.QUERY_DEVICE_STATUS + deviceId
        if appId != None:
            authUrl += "?appId=" + appId
        logging.info(deviceId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryDeviceRealtimeLocation(self, qdrlInDTO, appId, accessToken):
        qdrlInDTO = DictUtil.dto2dict(qdrlInDTO)
        authUrl = RestConstant.QUERY_DEVICE_REALTIME_LOCATION
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(qdrlInDTO)
        logging.info(qdrlInDTO), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    def queryDeviceShadow(self, deviceId, appId, accessToken):
        authUrl = RestConstant.QUERY_DEVICE_SHADOW + deviceId
        if appId != None:
            authUrl += "?appId=" + appId
        logging.info(deviceId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def modifyDeviceShadow(self, mdsInDTO, deviceId, appId, accessToken):
        mdsInDTO = DictUtil.dto2dict(mdsInDTO)
        authUrl = RestConstant.MODIFY_DEVICE_SHADOW + deviceId
        if appId != None:
            authUrl += "?appId=" + appId
        payload = json.dumps(mdsInDTO)
        logging.info(mdsInDTO), logging.info(deviceId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, payload, accessToken)

    # def regDirectDevice(self, clientInfo, rddInDto, appId, accessToken):
    #     rddInDto = DictUtil.dto2dict(rddInDto)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.REG_DIRECT_DEVICE
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(rddInDto)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def refreshDeviceKey(self, clientInfo, rdkInDTO, appId, accessToken):
    #     rdkInDTO = DictUtil.dto2dict(rdkInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo['platformPort'] + \
    #               RestConstant.REFRESH_DEVICE_KEY + rdkInDTO['deviceId']
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(rdkInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
    #
    # def modifyDeviceInfo(self, clientInfo, mdiInDto, deviceId, appId, accessToken):
    #     mdiInDto = DictUtil.dto2dict(mdiInDto)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.MODIFY_DEVICE_INFO + deviceId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(mdiInDto)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
    #
    # def deleteDirectDevice(self, clientInfo, deviceId, cascade, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.MODIFY_DEVICE_INFO + deviceId + "?"
    #     if appId != None:
    #         authUrl += "&appId=" + appId
    #     if cascade != None:
    #         authUrl += "&cascade=" + cascade
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPDELETE, authUrl, headers, None)
    #
    # def queryDeviceStatus(self, clientInfo, deviceId, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_STATUS + deviceId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def queryDeviceRealtimeLocation(self, clientInfo, qdrlInDTO, appId, accessToken):
    #     qdrlInDTO = DictUtil.dto2dict(qdrlInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_REALTIME_LOCATION
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(qdrlInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
    #
    # def queryDeviceShadow(self, clientInfo, deviceId, appId, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.QUERY_DEVICE_SHADOW + deviceId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, headers, None)
    #
    # def modifyDeviceShadow(self, clientInfo, mdsInDTO, deviceId, appId, accessToken):
    #     mdsInDTO = DictUtil.dto2dict(mdsInDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.MODIFY_DEVICE_SHADOW + deviceId
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(mdsInDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPUT, authUrl, headers, payload)
