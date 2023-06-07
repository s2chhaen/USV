import serial
import struct
from crc import CrcCalculator, Configuration
import math

#ReadWrite functionen brauchen alle eine serial connection und sind deswegen eher schwierig zu testen für die, die z.B. kein arduino haben
#zum local testen: ReadWriteCio

#---------------------------------------arbeits functionen fuer die benutzbaren funktionen

# def formatChar(dtpSize:int):    #dtp = data to process
#     formatString = ''

#     if dtpSize>1:
#         if dtpSize>2:
#             if dtpSize>4:
#                 formatString = 'q'      #long long int 8 byte
#             else:
#                 formatString = 'i'      #int 4 byte
#         else:
#             formatString = 'h'          #short 2 byte
#     else:
#         formatString = 'b'              #byte 1 byte

#     return formatString

def uformatChar(dtpSize:int):    #unsigned formatChar version
    formatString = ''

    if dtpSize>1:
        if dtpSize>2:
            if dtpSize>4:
                formatString = 'Q'      #long long int 8 byte
            else:
                formatString = 'I'      #int 4 byte
        else:
            formatString = 'H'          #short 2 byte
    else:
        formatString = 'B'              #byte 1 byte

    return formatString

def numberByteSize(num:int):
    if num == 1 or num == 0:
        size = 1
    else: 
        size =  int(math.ceil(math.log(num,2)/8))
    
    return size

def calculate_CRC(message:int): 
    # eigenschaften des DVB-S2 CRC systems
    width = 8
    poly = 0xd5
    initVal = 0x00
    finalXorVal = 0x00
    revIn = False
    revOut = False

    # config = Configuration(width,poly,initVal,finalXorVal,revIn,revOut)
    config = Configuration(width=width,polynomial=poly,init_value=initVal,final_xor_value=finalXorVal,reverse_input=revIn,reverse_output=revOut)

    useTabl = True  #use table

    crcCalcltr = CrcCalculator(config, useTabl)  #crc calculator instance

    mssgSize = numberByteSize(message)
    packdmsg = struct.pack('>'+ uformatChar(mssgSize),message)
    
    result = crcCalcltr.calculate_checksum(bytes(packdmsg))
    return result

def calculate_CRC_barr(message:bytearray):
    width = 8
    poly = 0xd5
    initVal = 0x00
    finalXorVal = 0x00
    revIn = False
    revOut = False

    config = Configuration(width,poly,initVal,finalXorVal,revIn,revOut)

    useTabl = True  #use table

    crcCalcltr = CrcCalculator(config, useTabl)  #crc calculator instance
    
    result = crcCalcltr.calculate_checksum(message)
    return result

#---------------------------------------benutzbare funktionen

def write(serIn:serial, sID:int, data:bytearray, adress:int):
    ResndAttempt = 0
    wAd = struct.pack("H",0x8000 + adress)                   #write mode + adress
    dataSize = len(data)
    flength = 7+ dataSize
    crcCode = calculate_CRC_barr(data)
    # fstr = '<2BHB'+formatChar(dataSize)+'2B'
    fstr = '<5B'
    frame = bytearray(struct.pack(fstr, 0xA5, sID, flength, crcCode, 0xA6))
    # frame = bytearray({0xA5, sID, flength, crcCode, 0xA6})
    frame[2:2] = wAd
    frame[5:5] = data

    while ResndAttempt <= 3:
        print("> sending: ")
        print(frame)

        serIn.write(frame)

        response = serIn.read(1)
        response = response[0]  #bytes->int conversion

        if response == 0xa1:
            print("> received ACK")
            return True
        elif response == 0xa2:
            ResndAttempt = ResndAttempt + 1
            print("> received NACK\nresend attempt #: " + str(ResndAttempt))
            continue
        else:
            print("> received " + str(response) + ", not recognised\nexiting function")
            return False

    print("> resend attempt limit reached, exiting function")
    return False

def read(serIn: serial, sID:int, datareqlength:int, readadress:int):    #datareqlength -> data request length in byte
    #send part
    ResndAttempt = 0
    wAd = 0x4000 + readadress       #read mode + adress
    crcCode = calculate_CRC(datareqlength)
    fstr = '<2BH4B'
    framesend = struct.pack(fstr, 0xA5, sID, wAd, 0x8, datareqlength, crcCode, 0xA6)  

    while ResndAttempt <= 3:
        print("> sending request: ")
        print(framesend)

        serIn.write(framesend)
        #receive part

        recmess = bytearray(serIn.read_until(b'\xa6'))   #received message

        fbyte = recmess[0]      #first byte of message
        
        data_content = 0

        if fbyte == 0xa5:
            print("> received: ")
            print(str(recmess))
            data_length = recmess[4]-7
            data_content = recmess[5:(5+data_length)]       #extracts data from frame
            recCRC = recmess[recmess[4]-2]
            calcCRC = calculate_CRC_barr(data_content)
            # print("crc :"+ hex(calcCRC))
            if calcCRC != recCRC:
                ResndAttempt = ResndAttempt + 1
                print("> wrong CRC\nresend attempt #: " + str(ResndAttempt))
                continue
            
            return data_content[::-1]
        elif fbyte == 0xa2:
            ResndAttempt = ResndAttempt + 1
            print("> received NACK\nresend attempt #: " + ResndAttempt)
            continue
        else:
            print("> received: " + str(recmess) + ", not recognized \nexiting function")
            return False

    print("> resend attempt limit reached\nexiting function")
    return False

    