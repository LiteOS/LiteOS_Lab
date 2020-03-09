import logging

from com.huawei.iotplatform.client.NorthApiClient import NorthApiClient
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.utils.DictUtil import DictUtil
from com.huawei.iotplatform.utils.LogUtil import Log


class DataCollection(object):
    log = Log()
    log.setLogConfig()

    def querySingleDeviceInfo(self, deviceId, select, appId, accessToken):
        authUrl = RestConstant.QUERY_SINGLE_DEVICE_INFO + deviceId + "?"
        if appId != None:
            authUrl += "appId=" + appId
        if select != None:
            authUrl += "&select=" + select
        logging.info(deviceId), logging.info(select), logging.info(appId), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryBatchDevicesInfo(self, qbdiInDTO, accessToken):
        qbdiInDTO = DictUtil.dto2dict(qbdiInDTO)
        authUrl = RestConstant.QUERY_BATCH_DEVICES_INFO
        for key in qbdiInDTO.keys():
            if qbdiInDTO[key] != None:
                authUrl += "&" + key + "=" + qbdiInDTO[key]
        logging.info(qbdiInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)
		
	def queryDeviceDataHistory(self, qddhInDTO, accessToken):
        authUrl = "/iocm/app/data/v1.2.0/deviceDataHistory"
        payload = DictUtil.dto2dict(qddhInDTO)
		logging.info(qddhInDTO), logging.info(accessToken), logging.info(payload)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, payload, accessToken)

    def queryDeviceDesiredHistory(self, qddhInDTO, accessToken):
        qddhInDTO = DictUtil.dto2dict(qddhInDTO)
        authUrl = RestConstant.QUERY_DEVICE_DESIRED_HISTORY + qddhInDTO['deviceId'] + "&gatewayId=" + qddhInDTO[
            'gatewayId']
        for key in qddhInDTO.keys():
            if qddhInDTO[key] != None:
                authUrl += "&" + key + "=" + qddhInDTO[key]
        logging.info(qddhInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)

    def queryDeviceCapabilities(self, qdcInDTO, accessToken):
        qdcInDTO = DictUtil.dto2dict(qdcInDTO)
        authUrl = RestConstant.QUERY_DEVICE_CAPABILITIES
        for key in qdcInDTO.keys():
            if qdcInDTO[key] != None:
                authUrl += "&" + key + "=" + qdcInDTO[key]
        logging.info(qdcInDTO), logging.info(accessToken)
        return NorthApiClient.invokeAPI(RestConstant.HTTPGET, authUrl, None, accessToken)
