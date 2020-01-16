import json
import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class DeviceServiceInvocation(object):
    log = Log()
    log.setLogConfig()

    def invokeDeviceService(self, commandDTO, deviceId, serviceId, appId, accessToken):
        commandDTO = DictUtil.dto2dict(commandDTO)
        authUrl = RestConstant.INVOKE_DEVICE_SERVICE + deviceId + "/services/" + serviceId + "/sendCommand"
        if appId != None:
            authUrl += "?appId=" + appId

        payload = json.dumps(commandDTO)
        logging.info(commandDTO), logging.info(deviceId), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, payload, accessToken)

    # def invokeDeviceService(self, clientInfo, commandDTO, deviceId, serviceId, appId, accessToken):
    #     commandDTO = DictUtil.dto2dict(commandDTO)
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + RestConstant.INVOKE_DEVICE_SERVICE + deviceId + "/services/" + serviceId + "/sendCommand"
    #     if appId != None:
    #         authUrl += "?appId=" + appId
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     payload = json.dumps(commandDTO)
    #     return NorthApiClient.invokeAPI(RestConstant.HTTPPOST, authUrl, headers, payload)
