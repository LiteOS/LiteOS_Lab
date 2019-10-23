import logging
import os
from logging.handlers import RotatingFileHandler

from com.huawei.iotplatform.constant.Constant import Constant


class Log(object):
    def setLogConfig(self,
                     logPath=os.path.join(Constant.workPath, 'log/'),
                     level=logging.DEBUG,
                     logFilename="python_sdk.log"):
        if level == 0:
            return
        if logPath == None:
            logPath = Constant.workPath
        logger = logging.getLogger()
        logger.setLevel(level)
        if os.path.exists(logPath):
            filename = str(logPath) + logFilename
        else:
            os.makedirs(logPath, 493)
            filename = str(logPath) + logFilename
        Rthandler = RotatingFileHandler(filename=filename, maxBytes=1024 * 512, backupCount=100000)
        formatter = logging.Formatter(datefmt='%Y-%m-%d %A %H:%M:%S',
                                      fmt='%(asctime)s  [%(levelname)s]:  %(funcName)s  %(message)s')
        Rthandler.setFormatter(formatter)
        logger.addHandler(Rthandler)

    def LogTest(self):
        log = Log()
        log.setLogConfig()

        # Print information
        logging.debug('logger debug-debug message')
        logging.info('logger info-info message')
        logging.warning('logger warning-warning message')
        logging.error('logger error-error message')
        logging.critical('logger critical-critical message')
        print("Log output succeeded!")


if __name__ == "__main__":
    log = Log()

    # LogTest
    log.LogTest()
