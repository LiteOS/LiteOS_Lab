class Headers(object):

    def __init__(self):
        self.app_key = None
        self.Authorization = None

    def getApp_key(self):
        return self.app_key

    def setApp_key(self, app_key):
        self.app_key = app_key

    def getAuthorization(self):
        return self.Authorization

    def setAuthorization(self, Authorization):
        self.Authorization = Authorization