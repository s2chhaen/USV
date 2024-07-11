# -*- coding: utf-8 -*-
"""
Created on Tue Dec 19 17:43:55 2023

@author: Grabow
"""

__version__ = '1.0'
__author__ = 'Joe Grabow'

X_SCALE = 111.19492664455873 * 1000
Y_SCALE = 68.4578930279912 * 1000


from math import cos, sin, atan2, sqrt, radians, degrees, log, tan, pi

"""Function to calculate the nautical distance (in km) between two given 
coordinates (Great Circle Distance)"""
def get_distance(lat1, long1, lat2, long2):
    try:
        lat1 = float(lat1)
        long1 = float(long1)
        lat2 = float(lat2)
        long2 = float(long2);
        R = 6371.0 # Radius of earth in metres
        lat1rads = radians(lat1)
        lat2rads = radians(lat2)
        deltalat = radians((lat2 - lat1))
        deltalong = radians((long2 - long1))
        a = sin(deltalat / 2) * sin(deltalat / 2) + cos(lat1rads) * cos(lat2rads) * sin(deltalong / 2) * sin(deltalong / 2)
        c = 2 * atan2(sqrt(a), sqrt(1 - a))
        d = R * c
        return d
    except Exception as e:
        return 'Exception: '+ e


"""Function to calculate the bearing between two given coordinates (in degree)"""
def get_bearing(lat1, long1, lat2, long2):
    try:
        lat1 = float(lat1)
        long1 = float(long1)
        lat2 = float(lat2)
        long2 = float(long2)
        startlat = radians(lat1)
        startlong = radians(long1)
        endlat = radians(lat2)
        endlong = radians(long2)
        dlong = endlong - startlong
        dPhi = log(tan(endlat / 2.0 + pi / 4.0) / tan(startlat / 2.0 + pi / 4.0))
        if abs(dlong) > pi:
            if dlong > 0.0:
                dlong = -(2.0 * pi - dlong)
            else:
                dlong = (2.0 * pi + dlong)
        bearing = (degrees(atan2(dlong, dPhi)) + 360.0) % 360.0;
        return bearing
    except Exception as e:
        return 'Exception: '+ e


"""Degrees Minutes (nautical notation, DD° MM.MMMM')"""
def decimal_to_dmm(decimal):
    try:
        decimal = float(decimal)
        degrees = int(decimal)
        minutes_decimal = (decimal - degrees) * 60
        minutes = int(minutes_decimal)
        seconds = (minutes_decimal - minutes) * 100  # 1/100 Minutes
        return degrees + minutes / 100.0 + seconds / 10000.0
    except Exception as e:
        return 'Exception: '+ e
    

""" APRS notation, DDMM.hhN/DDDMM.hhE"""     
def decimal_to_aprs(lat, long):
    try:
        latitude = decimal_to_dmm(lat) * 100
        longitude = decimal_to_dmm(long) * 100
            
        # Format: DDMM.hhN/DDDMM.hhE
        lat_a = format(latitude, '.2f')
        lon_a = f"{int(longitude):05d}.{int(longitude % 1 * 100):02d}"

            
        # Direction N or S, E or W
        lat_hemisphere = 'N' if latitude >= 0 else 'S'
        lon_hemisphere = 'E' if longitude >= 0 else 'W'
            
        # APRS-Format: DDMM.hhN/DDDMM.hhE
        aprs_string = f"{lat_a}{lat_hemisphere}/{lon_a}{lon_hemisphere}"
            
        return aprs_string
    
    except Exception as e:
        return 'Exception: '+ e
    
    
""" converts cartesian coordinates X_K, Y_K in m, into decimal degrees
    X_g0, Y_g0 is the reference point of cartesian coordinates
"""    
def decimal_geo(X_k, Y_k, X_g0, Y_g0):
    try:
        X_g = (X_k / X_SCALE) + X_g0
        Y_g = (Y_k / Y_SCALE) + Y_g0  
        return X_g, Y_g
    except Exception as e:
        return 'Exception: '+ e    
        
"""    
a, b = decimal_geo(50000.00, 50000, 52, 13)
print(a, b)
"""        