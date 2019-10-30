class SSLConfig(object):

    def __init__(self):
        self.selfCertPath = None
        self.selfCertPwd = None
        self.trustCAPath = None
        self.trustCAPwd = None

    def getSelfCertPath(self):
        return self.selfCertPath

    def setSelfCertPath(self, selfCertPath):
        self.selfCertPath = selfCertPath

    def getSelfCertPwd(self):
        return self.selfCertPwd

    def setSelfCertPwd(self, selfCertPwd):
        self.selfCertPwd = selfCertPwd

    def getTrustCAPath(self):
        return self.trustCAPath

    def setTrustCAPath(self, trustCAPath):
        self.trustCAPath = trustCAPath

    def getTrustCAPwd(self):
        return self.trustCAPwd

    def setTrustCAPwd(self, trustCAPwd):
        self.trustCAPwd = trustCAPwd
