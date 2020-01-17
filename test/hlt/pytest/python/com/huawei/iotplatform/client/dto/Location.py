class Location(object):

    def __init__(self):
        self.accuracy = None
        self.crashInformation = None
        self.description = None
        self.heading = None
        self.language = None
        self.latitude = None
        self.longitude = None
        self.numberOfPassengers = None
        self.region = None
        self.time = None
        self.vehicleSpeed = None

    def getAccuracy(self):
        return self.accuracy

    def setAccuracy(self, accuracy):
        self.accuracy = accuracy

    def getCrashInformation(self):
        return self.crashInformation

    def setCrashInformation(self, crashInformation):
        self.crashInformation = crashInformation

    def getDescription(self):
        return self.description

    def setDescription(self, description):
        self.description = description

    def getHeading(self):
        return self.heading

    def setHeading(self, heading):
        self.heading = heading

    def getLanguage(self):
        return self.language

    def setLanguage(self, language):
        self.language = language

    def getLatitude(self):
        return self.latitude

    def setLatitude(self, latitude):
        self.latitude = latitude

    def getLongitude(self):
        return self.longitude

    def setLongitude(self, longitude):
        self.longitude = longitude

    def getNumberOfPassengers(self):
        return self.numberOfPassengers

    def setNumberOfPassengers(self, numberOfPassengers):
        self.numberOfPassengers = numberOfPassengers

    def getRegion(self):
        return self.region

    def setRegion(self, region):
        self.region = region

    def getTime(self):
        return self.time

    def setTime(self, time):
        self.time = time

    def getVehicleSpeed(self):
        return self.vehicleSpeed

    def setVehicleSpeed(self, vehicleSpeed):
        self.vehicleSpeed = vehicleSpeed
