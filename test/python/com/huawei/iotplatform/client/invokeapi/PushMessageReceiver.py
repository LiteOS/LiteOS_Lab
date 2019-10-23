import logging

from flask import Flask, request
from requests import RequestException

from com.huawei.iotplatform.utils.LogUtil import Log

app = Flask(__name__)


@app.route('/v1.0.0/messageReceiver', methods=['POST'])
@app.route('/v1.0.0/messageReceiver/cmd', methods=['POST'])
def receive():
    log = Log()
    log.setLogConfig()

    pmr = PushMessageReceiver()
    try:
        if request.json['notifyType'] == "deviceAdded":
            logging.info(request.json)
            pmr.handleDeviceAdded()
            return "ok"

        if request.json['notifyType'] == "bindDevice":
            logging.info(request.json)
            pmr.handleBindDevice()
            return "ok"

        if request.json['notifyType'] == "deviceInfoChanged":
            logging.info(request.json)
            pmr.handleDeviceInfoChanged()
            return "ok"

        if request.json['notifyType'] == "deviceDataChanged":
            logging.info(request.json)
            pmr.handleDeviceDataChanged()
            return "ok"

        if request.json['notifyType'] == "deviceDatasChanged":
            logging.info(request.json)
            pmr.handleDeviceDatasChanged()
            return "ok"

        if request.json['notifyType'] == "serviceInfoChanged":
            logging.info(request.json)
            pmr.handleServiceInfoChanged()
            return "ok"

        if request.json['notifyType'] == "deviceDeleted":
            logging.info(request.json)
            pmr.handleDeviceDeleted()
            return "ok"

        if request.json['notifyType'] == "messageConfirm":
            logging.info(request.json)
            pmr.handleMessageConfirm()
            return "ok"

        if request.json['notifyType'] == "commandRsp":
            logging.info(request.json)
            pmr.handleCommandRsp()
            return "ok"

        if request.json['notifyType'] == "deviceEvent":
            logging.info(request.json)
            pmr.handleDeviceEvent()
            return "ok"

        if request.json['notifyType'] == "deviceModelAdded":
            logging.info(request.json)
            pmr.handleDeviceModelAdded()
            return "ok"

        if request.json['notifyType'] == "deviceModelDeleted":
            logging.info(request.json)
            pmr.handleDeviceDeleted()
            return "ok"

        if request.json['notifyType'] == "ruleEvent":
            logging.info(request.json)
            pmr.handleRuleEvent()
            return "ok"

        if request.json['notifyType'] == "deviceDesiredPropertiesModifyStatusChanged":
            logging.info(request.json)
            pmr.handleDeviceDesiredStatusChanged()
            return "ok"

        if request.json['notifyType'] == "swUpgradeStateChangeNotify":
            logging.info(request.json)
            pmr.handleSwUpgradeStateChanged()
            return "ok"

        if request.json['notifyType'] == "swUpgradeResultNotify":
            logging.info(request.json)
            pmr.handleSwUpgradeResult()
            return "ok"

        if request.json['notifyType'] == "fwUpgradeStateChangeNotify":
            logging.info(request.json)
            pmr.handleFwUpgradeStateChanged()
            return "ok"

        if request.json['notifyType'] == "fwUpgradeResultNotify":
            logging.info(request.json)
            pmr.handleFwUpgradeResult()
            return "ok"
        else:
            return "notifyType doesn't exist or notifyType is not right."
    except RequestException as e:
        logging.error(e)
        raise RequestException(e)


class PushMessageReceiver(object):

    def handleDeviceAdded(self):
        print("deviceAdded ==> ", request.json)
        pass

    def handleBindDevice(self):
        print("bindDevice ==> ", request.json)
        pass

    def handleDeviceInfoChanged(self):
        print("deviceInfoChanged ==> ", request.json)
        pass

    def handleDeviceDataChanged(self):
        print("deviceDataChanged ==> ", request.json)
        pass

    def handleDeviceDatasChanged(self):
        print("deviceDatasChanged ==> ", request.json)
        pass

    def handleServiceInfoChanged(self):
        print("serviceInfoChanged ==> ", request.json)
        pass

    def handleDeviceDeleted(self):
        print("deviceDeleted ==> ", request.json)
        pass

    def handleMessageConfirm(self):
        print("messageConfirm ==> ", request.json)
        pass

    def handleCommandRsp(self):
        print("commandRsp ==> ", request.json)
        pass

    def handleDeviceEvent(self):
        print("deviceEvent ==> ", request.json)
        pass

    def handleDeviceModelAdded(self):
        print("deviceModelAdded ==> ", request.json)
        pass

    def handleDeviceModelDeleted(self):
        print("deviceModelDeleted ==> ", request.json)
        pass

    def handleRuleEvent(self):
        print("ruleEvent ==> ", request.json)
        pass

    def handleDeviceDesiredStatusChanged(self):
        print("deviceDesiredPropertiesModifyStatusChanged ==> ", request.json)
        pass

    def handleSwUpgradeStateChanged(self):
        print("swUpgradeStateChangeNotify ==> ", request.json)
        pass

    def handleSwUpgradeResult(self):
        print("swUpgradeResultNotify ==> ", request.json)
        pass

    def handleFwUpgradeStateChanged(self):
        print("fwUpgradeStateChangeNotify ==> ", request.json)
        pass

    def handleFwUpgradeResult(self):
        print("fwUpgradeResultNotify ==> ", request.json)
        pass
