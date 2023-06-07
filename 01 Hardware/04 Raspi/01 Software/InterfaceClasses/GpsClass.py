import serial
from FunctionsToUse.ReadWrite import read
import numpy as nmp
    
def calculate_minus(binary_input): 
    binary_input = list(binary_input)
    binary_out = binary_input
    for epoch,i in enumerate(binary_out):
        if i == "0":
            binary_out[epoch] = "1"
        else:
            binary_out[epoch] = "0"
    s="".join(binary_out)
    _,out = bin(int(s,2)+1).split("b")
    return out

def caculate(message):
    # a1=message[5]
    # if a1>128:
    #     sign = -1               #minus
    # else:
    #     sign = 1
    # a2=message[6]
    # a3=message[7]
    # a4=message[8]               #Dezimalzahl für jede Byte
    # a1='{:08b}'.format(a1)
    # a2='{:08b}'.format(a2)
    # a3='{:08b}'.format(a3)
    # a4='{:08b}'.format(a4)      #Binärzahl für jede Byte
    # a1=str(a1)
    # a2=str(a2)
    # a3=str(a3)
    # a4=str(a4)                  #String für jede Byte
    # s=a1+a2+a3+a4               #gesamte String
    # s1=s[1:9]                   #runde Zahl Teil 
    # s2=s[9:26]                  #dezimale Zahl Teil 
    # if sign>0:
    #     num1 = int(s1,2)
    #     num2 = 0
    #     for i in range(17):
    #         num2 = num2+s2[i]*2**(-(i+1))
    #     result = (num1+num2)*sign
    # else:
    #     rs1 = calculate_minus(s1[1:])
    #     rs2 = calculate_minus(s2)
    #     num1 = int(rs1,2)
    #     num2 = 0
    #     for i in range(7):
    #         num2 = num2+int(rs2[i])*2**(-(i+1))
    #     result = -(num1+num2)

    # for i in range(4):
    #     print(bin(message[i]))
    result = message[0]*2+message[1]*pow(2, -7)+message[2]*pow(2, -15)+message[3]*pow(2, -23)
    # print(bin(result))

    return result

def get_position(Longitude_message,Lattitude_message):
    longitude = caculate(Longitude_message)
    lattitude = caculate(Lattitude_message)
    position_array = nmp.array([longitude,lattitude])
    return position_array

class gps:

    position = [0,0]
    __dataLength = 4

    def __init__(self, serialIn:serial,sID:int,Longitude_Adresse:int,Lattitude_Adresse:int):
        self.serialIn = serialIn
        self.__sID = sID
        self.__Longitude_Adresse = Longitude_Adresse 
        self.__Lattitude_Adresse = Lattitude_Adresse
        

    def update(self):
        Long_msg = read(self.serialIn, self.__sID, self.__dataLength, self.__Longitude_Adresse)
        Lat_msg = read(self.serialIn, self.__sID, self.__dataLength, self.__Lattitude_Adresse)
        self.position = get_position(Lat_msg, Long_msg)

