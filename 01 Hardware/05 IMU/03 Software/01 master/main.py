# coding: utf-8
"""
Project Created on Thu Oct 19 16:44:14 2023

Program history
19.10.2023    V. 1.0    Start
23.10.2023    V. 1.1    Modularisation

Package for IMU MPU9250
This is a MPU9250 library for the Pico-10DOF-IMU Rev2.1 I2C Brick.
The calculation is based on a Mahony Orientation Filter:
https://ahrs.readthedocs.io/en/latest/filters/mahony.html


@author: Joe Grabow (grabow@amesys.de)
"""
__version__ = '1.1'
__author__ = 'Joe Grabow'

import imu as imu


Gyro  = [0,0,0]
Accel = [0,0,0]
Mag   = [0,0,0]


gyroOffset=[0,0,0]
magOffset=[0,0,0]
pitch = 0.0
roll  = 0.0
yaw   = 0.0
Ki = 1.0
Kp = 4.50
q0 = 1.0
q1=q2=q3=0.0


mpu9250 = imu.MPU9250()

try:
    while True:
        mpu9250.readAccel()
        mpu9250.readGyro()
        mpu9250.readMagnet()
        mpu9250.imuAHRSupdate(Gyro[0]/32.8*0.0175, Gyro[1]/32.8*0.0175,Gyro[2]/32.8*0.0175,
                Accel[0],Accel[1],Accel[2], Mag[0], Mag[0], Mag[2])
        pitch = math.asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3
        roll  = math.atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3
        yaw   = math.atan2(-2 * q1 * q2 - 2 * q0 * q3, 2 * q2 * q2 + 2 * q3 * q3 - 1) * 57.3
        print("\r\n /-------------------------------------------------------------/ \r\n")
        print('\r\n Y Roll = %.2f , X Pitch = %.2f , Z Yaw = %.2f\r\n'%(roll,pitch,yaw))
        #print('\r\nAcceleration:  X = %d , Y = %d , Z = %d\r\n'%(Accel[0],Accel[1],Accel[2]))  
        #print('\r\nGyroscope:     X = %d , Y = %d , Z = %d\r\n'%(Gyro[0],Gyro[1],Gyro[2]))
        #print('\r\nMagnetic:      X = %d , Y = %d , Z = %d'%((Mag[0]),Mag[1],Mag[2]))
        time.sleep(0.1)

except KeyboardInterrupt:
    sys.exit()


