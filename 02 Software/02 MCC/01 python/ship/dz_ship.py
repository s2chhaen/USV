# -*- coding: utf-8 -*-
"""
Created on Mon Oct  2 10:06:46 2023

Program history
10.07.2024    V. 00.01    start virtual ship
11.07.2024    V. 00.02    with virtual akku and virtual drive
15.07.2024    V  00.03    MQTT Data

@author: Prof. Grabow (grabow@amesys.de)
"""
__version__ = '00.03'
__author__ = 'Joe Grabow'

import dz_gps  # virtual gps
import dz_drive  # virtual drive
import dz_akku  # virtual akku
import dz_data2packet  # generates compatible data for aprs transmitter 
import dz_data2mqtt  # generate and send MQTT data
#import shared_datablock  # data from bus

def get_data(delay, t_paket):
    dz_gps.get_data(delay)  # virtal gps
    dz_drive.get_data()  # virtual drive
    dz_akku.get_data(delay)  # virtual akku
    aprs_messages = dz_data2packet.get_data(t_paket)  # APRS Data
    dz_data2mqtt.get_data()  # generate and send QMTT data
       
    return aprs_messages


# main
if __name__ == "__main__":    
    t_paket = 0
    delay = 30
    aprs_messages = get_data(delay, t_paket)
  
    for i, frm in enumerate(aprs_messages, start=0):
        print(f"frm_{i}: {frm}")