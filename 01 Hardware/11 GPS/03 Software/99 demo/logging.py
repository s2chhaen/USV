# coding: utf-8
"""
Program history
08.11.2023    V. 1.0    Start
10.11.2023    V. 1.1    Calibration Data for the Magnetic Sensor
13.11.2024    V. 1.2    GPS Integration 

IMU measuring demo system for USV project
A calibration file "cal_data.json" is required.
The measured values of acceleration, gyroscope, magnetometer, and temperature are captured and output.
When the "j_flag" is set, magnetometer data is immediately compensated with calibration data during the
measurement.

https://github.com/Joe-Grabow/USV

@author: Joe Grabow (grabow@amesys.de)
"""
__version__ = '1.2'
__author__ = 'Joe Grabow'

import ujson
import utime
from machine import UART, Pin
from machine import I2C, Pin
from mpu9250 import MPU9250
from ak8963 import AK8963
from micropyGPS import MicropyGPS
from collections import OrderedDict  # Import für geordnete JSON-Struktur

# Instantiate the micropyGPS object
my_gps = MicropyGPS()

# Define the UART pins and create a UART object
gps_serial = UART(0, baudrate=9600, tx=Pin(0), rx=Pin(1), bits=8, parity=None, stop=1)


# load calibrate Data
def load_cal_data():
    with open('cal_data.json', 'r') as data:
        c_data = ujson.load(data)
    return(c_data)

# I2C port on Measurement-Board
i2c = I2C(1,scl=Pin(7),sda=Pin(6),freq=400_000)

# cal_data.json
# j_flag   0  sensor uncalibrated
#          1  sensor calibrated
# j_offset [x, y, z]
# j_scale  [x, y, z]
[j_flag, j_offset, j_scale] = load_cal_data()

if j_flag == 0:  # uncalibrated
    j_offset = [0 for _ in j_offset]  # all 0
    j_scale = [1 for _ in j_scale]  # all 1
else:  # use the calibrated data
    pass

dummy = MPU9250(i2c) # this opens the bybass to access to the AK8963

ak8963 = AK8963(
    i2c,
    offset=(j_offset[0], j_offset[1], j_offset[2]),
    scale=(j_scale[0], j_scale[2], j_scale[2])
)

sensor = MPU9250(i2c, ak8963=ak8963)

# Datei zum Schreiben öffnen
data_file = open('data_log.json', 'a')
# Hauptschleife zur Datenaufzeichnung
try:
    # Hauptschleife zur Datenaufzeichnung
    while True:
        # Sensorwerte auslesen
        ax, ay, az = sensor.acceleration
        gx, gy, gz = sensor.gyro

        # GPS-Daten lesen und aktualisieren
        if gps_serial.any():
            my_sentence = gps_serial.read()
            for byte in my_sentence:
                print('GPS-Empfang')
                stat = my_gps.update(chr(byte))
                if stat is not None:
                    Timestamp = my_gps.timestamp
                    Latitude = my_gps.latitude_string()
                    Longitude = my_gps.longitude_string()
                    Speed = my_gps.speed_string('kph')
                    Satellites = my_gps.satellites_in_use

                    # JSON-Daten als OrderedDict erstellen
                    data = OrderedDict([
                        ('ax', ax),
                        ('ay', ay),
                        ('az', az),
                        ('gx', gx),
                        ('gy', gy),
                        ('gz', gz),
                        ('Timestamp', Timestamp),
                        ('Latitude', Latitude),
                        ('Longitude', Longitude),
                        ('Speed', Speed),
                        ('Satellites', Satellites)
                    ])

                    # Daten in die JSON-Datei anhängen
                    ujson.dump(data, data_file)
                    data_file.write('\n')

                    # Verzögerung für die nächste Aufzeichnung
                    utime.sleep(1)

except KeyboardInterrupt:
    # Ausnahme fängt Ctrl-C ab und schließt die Datei
    print("Programm durch Ctrl-C beendet. Schließe die Datei.")
    data_file.close()