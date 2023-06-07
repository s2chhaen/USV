import numpy as nmp
from geographiclib.geodesic import Geodesic


def cart_dist(A, B):

    #Geodesic.WGS84.Inverse(A[0], A[1], B[0], B[1])

    g = Geodesic.WGS84.Inverse(A[0], A[1], B[0], B[1])

    dist = g['s12']
    
    #geopy
    #dist = distance.distance(A, B).m
    
    return dist

def cart_ang(A, B, comp):

    # WGS84
    # lat[0] long[1]
    
    g = Geodesic.WGS84.Inverse(A[0], A[1], B[0], B[1])


    theta = g['azi1']

    brng = 0
    if theta <= 0:
        brng = 360 + theta


    dir = brng - comp
    
    ### valid values only between 0 and 90 AND 270 and 0
        
    return dir

def cart2pol(A, B, comp):
    #polar
    
    temp_pol = nmp.array([0,0])
    
    temp_pol = nmp.array([cart_dist(A, B), cart_ang(A, B, comp)])
    
    return temp_pol



def corr(sector, lock, offset):
         
    pos_count = 0
    neg_count = 0
    
    for i in range(sector, 361, 1):
        
        if lock[i + offset] != False:
            
            pos_count = pos_count + 1
            
            TRUE_flag = True
            
        elif TRUE_flag != False:
            
                break

    for i in range(sector, 0, -1):
        
        if lock[i - offset] != False:
            
            neg_count = neg_count + 1
            
            TRUE_flag = True
            
        elif TRUE_flag != False:
            
                break
            
    # # print(pos_count)
    # # print(neg_count)
    
    if pos_count > neg_count:
        corr_sector = sector + offset
    elif neg_count > pos_count:
        corr_sector = sector - offset
    else:
        pos_count + nmp.random.randint(-1,1)
        
        if pos_count > neg_count:
            corr_sector = sector + offset
        elif neg_count > pos_count:
            corr_sector = sector - offset

    print('corr_sector: ', corr_sector)
            
    corr_ang = (corr_sector - 180) / 2 
        
    return corr_ang


def direct(gps, wp, comp):
    
    new_wp = nmp.zeros([2])
    
    # WGS84
    # phi = lat     [0]
    # lamda = long  [1]

    if wp[1] + comp - 360 <= 0:
        g = Geodesic.WGS84.Direct(gps[0], gps[1], wp[1]+comp-360, wp[0])
    else:
        g = Geodesic.WGS84.Direct(gps[0], gps[1], wp[1]+comp, wp[0])
    
    new_wp[0] = g['lat2']
    new_wp[1] = g['lon2']
    
    return new_wp