# coding: utf-8
"""
Program history
08.11.2023    V. 1.0    Start

IMU measuring demo system for USV project

https://github.com/Joe-Grabow/USV

@author: Joe Grabow (grabow@amesys.de)
"""
__version__ = '1.0'
__author__ = 'Joe Grabow'

import ujson
import utime
from machine import I2C, Pin
from mpu9250 import MPU9250
from ak8963 import AK8963

# load calibrate Data
def load_cal_data():
    with open('cal_data.json', 'r') as data:
        c_data = ujson.load(data)
    return(c_data)

# I2C port on Measurement-Board
i2c = I2C(1,scl=Pin(7),sda=Pin(6),freq=400_000)

[j_offset, j_scale] = load_cal_data()

dummy = MPU9250(i2c) # this opens the bybass to access to the AK8963

ak8963 = AK8963(
    i2c,
    offset=(j_offset[0], j_offset[1], j_offset[2]),
    scale=(j_scale[0], j_scale[1], j_scale[2])
)
sensor = MPU9250(i2c)

print("MPU9250 id: " + hex(sensor.whoami))



while True:
    print(sensor.acceleration)
    print(sensor.gyro)
    print(sensor.magnetic)
    print(sensor.temperature)

    utime.sleep_ms(1000)