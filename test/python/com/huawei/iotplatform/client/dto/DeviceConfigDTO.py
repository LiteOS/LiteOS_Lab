from com.huawei.iotplatform.client.dto.DataConfigDTO import DataConfigDTO


class DeviceConfigDTO(object):
    # dataConfig = "dataConfig"

    def __init__(self):
        self.dataConfig = DataConfigDTO
        self.dataConfig = "dataConfig"

    def getDataConfig(self):
        return self.dataConfig

    def setDataConfig(self, dataConfig):
        self.dataConfig = dataConfig
