from com.huawei.iotplatform.client.dto.OperateDevices import OperateDevices
from com.huawei.iotplatform.client.dto.OperatePolicy import OperatePolicy
from com.huawei.iotplatform.client.dto.OperationStaResult import OperationStaResult


class OperationInfo(object):
    targets = OperateDevices()
    policy = OperatePolicy()
    staResult = OperationStaResult()

    def __init__(self):
        self.operationId = None
        self.createTime = None
        self.startTime = None
        self.stopTime = None
        self.operateType = None

        self.status = None

        self.extendPara = None

    def getOperationId(self):
        return self.operationId

    def setOperationId(self, operationId):
        self.operationId = operationId

    def getCreateTime(self):
        return self.createTime

    def setCreateTime(self, createTime):
        self.createTime = createTime

    def getStartTime(self):
        return self.startTime

    def setStartTime(self, startTime):
        self.startTime = startTime

    def getStopTime(self):
        return self.stopTime

    def setStopTime(self, stopTime):
        self.stopTime = stopTime

    def getOperateType(self):
        return self.operateType

    def setOperateType(self, operateType):
        self.operateType = operateType

    def getStatus(self):
        return self.status

    def setStatus(self, status):
        self.status = status

    def getExtendPara(self):
        return self.extendPara

    def setExtendPara(self, extendPara):
        self.extendPara = extendPara

    def getTargets(self):
        return self.targets

    def setTargets(self, targets):
        self.targets = targets

    def getPolicy(self):
        return self.policy

    def setPolicy(self, policy):
        self.policy = policy

    def getStaResult(self):
        return self.staResult

    def setStaResult(self, staResult):
        self.staResult = staResult
