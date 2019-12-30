import configparser
import os

from com.huawei.iotplatform.client.dto.ClientInfo import ClientInfo
from com.huawei.iotplatform.utils.DictUtil import DictUtil

cf = configparser.ConfigParser()


class Constant(object):
    workPath = os.path.join('e:/src/eclipse_workspace/LiteOS_reviewed_test/LiteOS_Lab/test/python/')

    def readCertificate(self):
        certFilePath = os.path.join(Constant.workPath, 'cert/client.crt')
        certFilePath2 = os.path.join(Constant.workPath, 'cert/client.key')
        cert = (certFilePath, certFilePath2)
        return cert

    def readConfFile(self):
        configFilePath = os.path.join(Constant.workPath, 'resources/application.ini')
        cf.read(configFilePath)
        platformIp = cf.get("CLIENT_INFO", "platformIp")
        platformPort = cf.get("CLIENT_INFO", "platformPort")
        appId = cf.get("CLIENT_INFO", "appId")
        secret = cf.get("CLIENT_INFO", "secret")
        return platformIp, platformPort, appId, secret

    def readConfFileEx(self, client_title):
        configFilePath = os.path.join(Constant.workPath, 'resources/application.ini')
        cf.read(configFilePath)
        platformIp = cf.get(client_title, "platformIp")
        platformPort = cf.get(client_title, "platformPort")
        appId = cf.get(client_title, "appId")
        secret = cf.get(client_title, "secret")
        return platformIp, platformPort, appId, secret

    def clientInfo(self):
        clientInfo = ClientInfo()
        clientInfo.setPlatformIp((Constant().readConfFile())[0])
        clientInfo.setPlatformPort((Constant().readConfFile())[1])
        clientInfo.setAppId((Constant().readConfFile())[2])
        clientInfo.setSecret((Constant().readConfFile())[3])
        clientInfo = DictUtil.dto2dict(clientInfo)
        return clientInfo

    def clientInfoEx(self, client_title):
        clientInfo = ClientInfo()
        clientInfo.setPlatformIp((Constant().readConfFileEx(client_title))[0])
        clientInfo.setPlatformPort((Constant().readConfFileEx(client_title))[1])
        clientInfo.setAppId((Constant().readConfFileEx(client_title))[2])
        clientInfo.setSecret((Constant().readConfFileEx(client_title))[3])
        clientInfo = DictUtil.dto2dict(clientInfo)
        return clientInfo

    # def clientInfo(self):
    #     clientInfo = ClientInfo()
    #     clientInfo.platformIp = (Constant().readConfFile())[0]
    #     clientInfo.platformPort = (Constant().readConfFile())[1]
    #     clientInfo.appId = (Constant().readConfFile())[2]
    #     clientInfo.secret = (Constant().readConfFile())[3]
    #     clientInfo = DictUtil.dto2dict(clientInfo)
    #     return clientInfo

    ######################################------------ Authentication ------------######################################
    #####################################------------ DeviceManagement ------------#####################################
    #######################################------------ BatchProcess ------------#######################################
    ######################################------------ RuleEngine ------------##########################################
    #################################------------ SubscriptionManagement ------------###################################
    #################################------------ DeviceServiceInvocation ------------##################################
    ####################################------------ SignalDelivery ------------########################################
    ######################################------------ DataCollection ------------######################################
    ##################################------------ DeviceGroupManagement ------------###################################
    ######################################------------ DeviceUpgrade ------------#######################################
    ###################################------------ PushMessageReceiver ------------####################################
