from com.huawei.iotplatform.client.dto.CoordinateReferenceSystem import CoordinateReferenceSystem


class QueryDeviceRealtimeLocationInDTO(object):
    geoInfo = CoordinateReferenceSystem

    def __init__(self):
        self.horAcc = int

    def getHorAcc(self):
        return self.horAcc

    def setHorAcc(self, horAcc):
        self.horAcc = horAcc

    def getGeoInfo(self):
        return self.geoInfo

    def setGeoInfo(self, geoInfo):
        self.geoInfo = geoInfo
