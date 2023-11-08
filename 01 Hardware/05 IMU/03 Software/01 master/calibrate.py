# coding: utf-8
"""
Program history
08.11.2023    V. 1.0    Start

IMU measuring system for USV project
calibrate the AK9863 an save data in cal_data.json

https://github.com/Joe-Grabow/USV

@author: Joe Grabow (grabow@amesys.de)
"""
__version__ = '1.0'
__author__ = 'Joe Grabow'

import ujson
from machine import I2C, Pin
from mpu9250 import MPU9250
from ak8963 import AK8963

# save calibrate Data
def save_cal_data(c_data):
    with open('cal_data.json', 'w') as data:
        ujson.dump(c_data, data)

i2c = I2C(1,scl=Pin(7),sda=Pin(6),freq=400_000)

dummy = MPU9250(i2c) # this opens the bybass to access to the AK8963
ak8963 = AK8963(i2c)
offset, scale = ak8963.calibrate(count=256, delay=200)

sensor = MPU9250(i2c, ak8963=ak8963)

cal_data = [offset, scale]
save_cal_data(cal_data)
print(offset, scale)