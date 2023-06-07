import serial
import math
from FunctionsToUse.ReadWrite import read

class radar:
    distance = 0
    AoE = 0
    speed = 0
    direction = 0

    def __init__(self, serialIn:serial, sID:int, distadr:int, speedadr:int):
        self.serialIn = serialIn
        self.DistanceAdresse = distadr
        self.SpeedAdresse = speedadr
        self.SlaveID = sID

    def update(self):

        # Entfernung des Objektes
        input = read(self.serialIn, self.SlaveID, 1, self.DistanceAdresse)
        self.distance = input[0]

        # Geschwindigkeit des Objektes
        input2 = read(self.serialIn, self.SlaveID, 2, self.SpeedAdresse)
        speedstr = input2[0] + (input2[1]/256)                             #nachkomma stelle muss mit 2^-n
        self.speed = float(speedstr)
        if self.speed >= 128:
            self.speed = 256 - self.speed

        # Bewegungsrichtung des Objektes
        if self.distance >= 128:
            self.direction = 1
        else: 
            self.direction = 0

        # Sperrbereich
        if self.direction >= 1:
            # reale Distanz, geschwindigkeitsabhaenig 
            distreal = self.distance - self.speed * 2 # ausgehend von einer Zeitdifferenz von 2s zwischen Messungen
            # AoE ist Anzahl der Bereiche, die in jede Richtung zu sperren sind
            self.AoE = 2 * math.asin(17.4 / distreal) # ausgehend von einem Öffnungswinkel von 20 Grad und max. Distanz von 100m
            round(self.AoE, 1) # auf ganze Zahl gerundet
        else: self.AoE = 0