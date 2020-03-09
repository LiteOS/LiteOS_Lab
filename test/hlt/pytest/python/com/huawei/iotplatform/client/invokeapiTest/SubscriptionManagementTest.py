import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.SubscriptionManagement import \
    SubscriptionManagement

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.DeleteBatchSubInDTO import DeleteBatchSubInDTO
from com.huawei.iotplatform.client.dto.QueryBatchSubInDTO import QueryBatchSubInDTO
from com.huawei.iotplatform.client.dto.SubDeviceBusinessDataInDTO import SubDeviceBusinessDataInDTO
from com.huawei.iotplatform.client.dto.SubDeviceManagementDataInDTO import SubDeviceManagementDataInDTO
from com.huawei.iotplatform.client.dto.SubscriptionDTO import SubscriptionDTO
from com.huawei.iotplatform.constant.Constant import Constant


class SubscriptionManagementTest(object):
    def subDeviceBusinessData(self):
        sdbdInDTO = SubDeviceBusinessDataInDTO()
        sdbdInDTO.notifyType = "bindDevice"
        sdbdInDTO.callbackUrl = "https://10.63.166.253:8099/v1.0.0/messageReceiver"
        return sdbdInDTO

    def subDeviceManagementData(self):
        sdmdInDTO = SubDeviceManagementDataInDTO()
        sdmdInDTO.notifyType = "swUpgradeStateChangeNotify"
        sdmdInDTO.callbackurl = "https://10.63.166.253:8099/v1.0.0/messageReceiver"
        return sdmdInDTO

    def queryBatchSubscriptions(self):
        qbsInDTO = QueryBatchSubInDTO()
        qbsInDTO.appId = "3RQ9UnhymV409MfKPuiin75XroQa"
        qbsInDTO.notifyType = "deviceDataChanged"
        qbsInDTO.pageNo = "1"
        qbsInDTO.pageSize = "1"
        return qbsInDTO

    def deleteBatchSubscriptions(self):
        dbsInDTO = DeleteBatchSubInDTO()
        dbsInDTO.appId = "3RQ9UnhymV409MfKPuiin75XroQa"
        dbsInDTO.callbackUrl = "https://10.63.166.253:8099/v1.0.0/messageReceiver"
        dbsInDTO.notifyType = "deviceAdded"
        return dbsInDTO


if __name__ == "__main__":
    smTest = SubscriptionManagementTest()
    authentication = Authentication()
    subscriptionManagement = SubscriptionManagement()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # sub deviceAdded notification
    ss = subscriptionManagement.subDeviceBusinessData(smTest.subDeviceBusinessData(), accessToken)
    print("====== subscribe to device business data notification ======")
    print("result:", ss + "\n")

    # get subscriptionId
    rddod = SubscriptionDTO()
    rddod.setSubscriptionId(json.loads(ss)['subscriptionId'])
    subscriptionId = rddod.getSubscriptionId()
    print("subscriptionId==", subscriptionId + "\n")

    # sub device upgrade result notification
    ss = subscriptionManagement.subDeviceManagementData(smTest.subDeviceManagementData(), accessToken)
    print("====== subscribe to device management data notification ======")
    print("result:", ss + "\n")

    # query single subscription
    sq = subscriptionManagement.querySingleSubscription(subscriptionId, None, accessToken)
    print("====== query single subscription ======")
    print("result:", sq + "\n")

    # delete single subscription
    sd = subscriptionManagement.deleteSingleSubscription(subscriptionId, None, accessToken)
    print("====== delete single subscription ======")
    print("result:", sd)
    print("- delete single subscription succeeds -" + "\n")

    # query batch subscriptions
    sq = subscriptionManagement.queryBatchSubscriptions(smTest.queryBatchSubscriptions(), accessToken)
    print("====== query batch subscriptions ======")
    print("result:", sq + "\n")

    # delete batch subscriptions
    sd = subscriptionManagement.deleteBatchSubscriptions(smTest.deleteBatchSubscriptions(), accessToken)
    print("====== delete batch subscriptions ======")
    print("result:", sd)
    print("- delete batch subscriptions succeeds -")
