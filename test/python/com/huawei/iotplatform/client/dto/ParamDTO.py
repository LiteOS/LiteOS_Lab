from com.huawei.iotplatform.client.dto.CommandDTOV4 import CommandDTOV4


class ParamDTO(object):
    # command = "command "
    command = CommandDTOV4

    fileId = "fileId"
    deviceList = "deviceList"
    type = "type"
    def __init__(self):

        self.deviceType = str
        self.manufacturerId = str
        self.model = str
        self.deviceLocation = str
        self.groupList = list
        self.callbackUrl = int
        self.maxRetransmit = int
#1