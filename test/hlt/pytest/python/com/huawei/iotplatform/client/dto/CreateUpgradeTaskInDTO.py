from com.huawei.iotplatform.client.dto.OperateDevices import OperateDevices
from com.huawei.iotplatform.client.dto.OperatePolicy import OperatePolicy


class CreateUpgradeTaskInDTO(object):
    targets = OperateDevices()
    policy = OperatePolicy()

    def __init__(self):
        self.fileId = None
        self.targets = None
        self.policy = None

    def getFileIde(self):
        return self.fileId

    def setFileId(self, fileId):
        self.fileId = fileId

    def getTargets(self):
        return self.targets

    def setTargets(self, targets):
        self.targets = targets

    def getPolicy(self):
        return self.policy

    def setPolicy(self, policy):
        self.policy = policy
