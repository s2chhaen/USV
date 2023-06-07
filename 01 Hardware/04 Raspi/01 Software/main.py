import sys
import time
import serial
from FunctionsToUse.ReadWrite import write
from FunctionsToUse.BinFunctions import *
from FunctionsToUse.CartFunctions import cart_dist
from InterfaceClasses.LidarClass import lidar
from InterfaceClasses.RadarClass import radar
from InterfaceClasses.GpsClass import gps
from InterfaceClasses.CompassClass import kompass
from InterfaceClasses.MissionCTRLClass import mission



def erreichen(index, position, missionC:mission):   # index der Zielpunkt; position ist von GPS Klasse
    distance = cart_dist(position,missionC.punktB)
    if distance < missionC.epsilon:
        new_index=index+1
        return new_index
    else:
        return index

def dataSend(serialIn:serial, sID, punktA, punktB, avgSpeed, epsilon, punktAadr):

    A_message=positionToFixed(punktA[::-1])
    # write(serialIn, sID,A_message,positionAdr)
    B_message=positionToFixed(punktB[::-1])
    C_message=floatToFixed(avgSpeed,(8,8))
    D_message=floatToFixed(epsilon,(8,8))
    
    write(serialIn, sID,A_message+B_message+C_message+D_message,punktAadr) # write punktA, punktB, speed setpoint und epsilon schlauch

def combLiRabin(Li:lidar,Ra:radar,lval):
    Cdist = Li.distances                 #combined distances
    AoE = Ra.AoE*(lval/180)
    loc1 = int(lval/2)-int(AoE/2)
    loc2 = int(lval/2)+int(AoE/2)
    radarDistances = [Ra.distance]*(loc2-loc1)
    Cdist[loc1:loc2] = radarDistances
    # bool array mit "gesperrten" bereichen
    boolArray = [0]*len(Cdist)
    for i,v in enumerate(Cdist):
        boolArray[i] = (v<80)
    return boolArray

def main():

    # serial initialization

    Port = 'COM9' #'/dev/serial0'
    Baudrate = 250000

    serialIO = serial.Serial(port=Port, baudrate=Baudrate, parity = serial.PARITY_ODD, stopbits = serial.STOPBITS_ONE, timeout = 5)

    #class initialization and constants definition
    SlaveID = 0
    
    totalTime = 5   #zeit die ein durchlauf dauern soll

    # start adressen
    lidarAdr =  0x203
    distAdr =   0x012     # radar adresse 1
    speedAdr =  0x013     # radar adresse 2
    satFixAdr = 0x009     # satfix adresse 
    LongAdr =   0x001     # gps adresse 1
    LatAdr =    0x005     # gps adresse 2
    kompAdr =   0x00c    
    punktAAdr = 0x100     # output adress 1
    punktBAdr = 0x108     # output adress 2
    OavgSpdAdr =   0x110    # output adress 3 mittlere geschwindigkeit
    OepsilonAdr =  0x112    # output adress 4 epsilon schlauch

    LidarValues = 360

    myLidar =   lidar(serialIO, sID=SlaveID, adrs=lidarAdr, dataReqLength=LidarValues)
    myRadar =   radar(serialIO, sID=SlaveID, distadr=distAdr, speedadr=speedAdr)
    myGps =     gps(serialIO, sID=SlaveID, Longitude_Adresse=LongAdr, Lattitude_Adresse=LatAdr)
    myComp =    kompass(serialIO, sID=SlaveID, adrs=kompAdr)
    myMctl =    mission('InterfaceClasses/mission.txt') # "mission.txt" = name von mission dokument
    
    index = 0

    while True:
        starttime = time.time()

        #--------------------------------input readin
        myLidar.update()
        print(">>> Lidar update done\n")
        myRadar.update()
        print(">>> Radar update done\n")
        myGps.update()
        print("Pos"+ str(myGps.position))
        print(">>> GPS update done\n")
        myComp.update()
        print("Winkel: "+ str(myComp.winkel))
        print(">>> Compass update done\n")
        #--------------------------------Kombination Lidar-Radar inputs
        LockedZones = combLiRabin(myLidar,myRadar,LidarValues) #Lidar Radar data combination
        index = erreichen(index,myGps.position,myMctl)
        print("\n>> waypoint index = " + str(index) + "\n")
        #--------------------------------Mission control update mit modifizierten Daten
        myMctl.update(myGps.position, myComp.winkel, index, LockedZones)
        print(">>> Mission Control update done")

        #--------------------------------absenden der daten an den Regler
        print("\nprevious Waypoint: " + str(myMctl.punktA))
        print("next Waypoint: " + str(myMctl.punktB))
        print("Speed setpoint: " + str(myMctl.speed))
        print("Epsilon width: " + str(myMctl.epsilon) + "\n")
        dataSend(serialIO, SlaveID, myMctl.punktA, myMctl.punktB, myMctl.speed, myMctl.epsilon, punktAAdr)
        print("\n------------------------------------------------\n")

        if myMctl.punktB[0] == myMctl.punktA[0] and myMctl.punktB[1] == myMctl.punktA[1] and not index == 0:
            break

        endtime = time.time()

        if totalTime-(endtime-starttime) <= totalTime and totalTime-(endtime-starttime) >= 0:
            time.sleep(totalTime-(endtime-starttime))




if __name__ == '__main__':
    sys.exit(main())
    


