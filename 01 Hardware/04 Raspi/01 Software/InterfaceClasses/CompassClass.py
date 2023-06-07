import serial
from FunctionsToUse.ReadWrite import read

def caculate_winkel(message):
    # a1=message[5]
    # a2=message[6]           #Dezimalzahl für jede Byte 
    # a1='{:08b}'.format(a1)
    # a2='{:08b}'.format(a2)  #Binärzahl für jede Byte 
    # a1=str(a1)
    # a2=str(a2)              #String für jede Byte
    # s=a1+a2                 #gesamte String
    # s1=s[:9]                #runde Zahl Teil 
    # s2=s[9:]                #dezimale Zahl Teil 
    # num1 = int(s1,2)
    # num2 = 0
    # for i in range(7):
    #     num2 = num2+s2[i]*2**(-(i+1))
    # winkel = num1+num2

    winkel = message[0]*2+message[1]*pow(2,-7)

    return winkel

class kompass:

    winkel = 0
    __Length = 2

    def __init__(self, serialIn:serial, sID:int,adrs:int):
        self.serialIn = serialIn
        self.__sID = sID
        self.__Adresse = adrs
    
    def update(self):
        self.message = read(self.serialIn,self.__sID,self.__Length,self.__Adresse)
        self.winkel = caculate_winkel(self.message)