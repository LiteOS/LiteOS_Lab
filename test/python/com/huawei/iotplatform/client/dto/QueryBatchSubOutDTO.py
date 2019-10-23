from com.huawei.iotplatform.client.dto.SubscriptionDTO import SubscriptionDTO


class QueryBatchSubOutDTO(object):

    def __init__(self):
        self.totalCount = None
        self.pageNo = None
        self.pageSize = None
        self.subscriptions = SubscriptionDTO

    def getTotalCount(self):
        return self.totalCount

    def setTotalCount(self, totalCount):
        self.totalCount = totalCount

    def getPageNo(self):
        return self.pageNo

    def setPageNo(self, pageNo):
        self.pageNo = pageNo

    def getPageSize(self):
        return self.pageSize

    def setPageSize(self, pageSize):
        self.pageSize = pageSize

    def getSubscriptions(self):
        return self.subscriptions

    def setSubscriptions(self, subscriptions):
        self.subscriptions = subscriptions
