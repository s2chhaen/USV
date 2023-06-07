import math

def floatToFixed(num, QC:tuple): #QC = Qcode (als array eingeben)
    
    # formt float in Festkommazahl um
    
    size =  int(math.ceil((QC[0]+QC[1])/8))

    num = int(num*pow(2,size*8-QC[0]))
    
    result = bytearray(size)

    for i in range(size):
        result[i] = num & 0x000000ff
        num = num >> 8

    return result


def positionToFixed(array):
    x=array[0]
    y=array[1]
    x_bin=floatToFixed(x,(9,17))
    y_bin=floatToFixed(y,(9,17))
    result=x_bin+y_bin
    # int_result=int.from_bytes(result,'big')
    return result