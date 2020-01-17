class OperationStaResult(object):

    def __init__(self):
        self.total = None
        self.wait = None
        self.processing = None
        self.success = None
        self.fail = None
        self.stop = None
        self.timeout = None

    def getTotal(self):
        return self.total

    def setTotal(self, total):
        self.total = total

    def getWait(self):
        return self.wait

    def setWait(self, wait):
        self.wait = wait

    def getProcessing(self):
        return self.processing

    def setProcessing(self, processing):
        self.processing = processing

    def getSuccess(self):
        return self.success

    def setSuccess(self, success):
        self.success = success

    def getFail(self):
        return self.fail

    def setFail(self, fail):
        self.fail = fail

    def getStop(self):
        return self.stop

    def setStop(self, stop):
        self.stop = stop

    def getTimeout(self):
        return self.timeout

    def setTimeout(self, timeout):
        self.timeout = timeout
