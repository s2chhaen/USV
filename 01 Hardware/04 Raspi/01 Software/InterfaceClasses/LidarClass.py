import serial
from FunctionsToUse.ReadWrite import read


class lidar:

    def __init__(self, serIn:serial, sID:int, adress:int, dataReqLength:int):
        self.serialIn = serIn
        self.__slaveID = sID
        self.__adressStart = adress
        self.__dataReqLength = dataReqLength
        self.distances = [0]*dataReqLength

    
    def update(self):
        distances = read(self.serialIn, self.__slaveID, int(self.__dataReqLength/2), self.__adressStart)
        distances += read(self.serialIn, self.__slaveID, int(self.__dataReqLength/2), self.__adressStart+180)
        
        for i,v in enumerate(distances):
            self.distances[i] = v/2

        # daten kommen im format > dist*2 (kein fixed point, daten auf halben meter gerundet)
