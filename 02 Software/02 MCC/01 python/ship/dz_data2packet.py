# -*- coding: utf-8 -*-
"""
Created on Fri Jul 12 12:34:56 2024

Program history
12.07.2024    V. 00.01    start

read all data from bus (shared_datablock) and 
generates compatible data for aprs transmitter

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.01'
__author__ = 'Joe Grabow'

import shared_datablock
from nautic import decimal_to_aprs

Secondary_Symbol_Key = 's'  # Ship 


def create_byte(x, y):
    """ first nibble x, 2nd nibble y """
    if not (0 <= x <= 15) or not (0 <= y <= 15):
        raise ValueError("x and y must be between 0 and 15.")
    byte = (x << 4) | y
    return byte


def telemetry_report(snr, a1, a2, a3, a4, a5, d1):
    """ telemetry report without comment
        snr - sequential number (Integer)
        a1 ... a5 - analog value (Integer)
        d1 - digital value (Integer)
    """
    number = str(int(snr)).zfill(3) 
    analog_1 = str(int(a1)).zfill(3)  # 3 digits 
    analog_2 = str(int(a2)).zfill(3)  
    analog_3 = str(int(a3)).zfill(3)   
    analog_4 = str(int(a4)).zfill(3)   
    analog_5 = str(int(a5)).zfill(3)  
    bin_number = bin(d1)  # binär with '0b' Präfix
    digital = bin_number[2:].zfill(8)  # without '0b' an exact 8 digits
    result =f'T#{number},{analog_1},{analog_2},{analog_3},{analog_4},{analog_5},{digital}'
    return result


def position_report(lat, long, bearing, velocity_kn):
    """ positions report without comments
        latitude in decimal degree (float)
        longitude in decimal degree (float)
        bearing in degree (float)
        velocity_kn in knots (float)
    """
    aprs_1 = decimal_to_aprs(lat, long)  # positions data
    aprs_2 = str(round(bearing))  # bearing data
    aprs_3 = str(int(velocity_kn)).zfill(3)  # velocity data
    result = f'{aprs_1}{Secondary_Symbol_Key}{aprs_2}/{aprs_3}'
    return result

 
def get_data(t_paket):    
        
    # Telemetry report calculations
    calculations_report_1 = [
        lambda db: db['EM1'] * 10,              # voltage akku 1
        lambda db: db['EM2'] * 10 / 2,          # current akku 1
        lambda db: db['EM9'] * 10,              # voltage lidar       
        lambda db: db['EM7'] * 10,              # voltage solar
        lambda db: db['EM8'] * 10 / 2,          # board temperature 
        lambda db: (int(db['SB1']) >> 4) & 0x0F # first nibble from SB1
    ]

    calculations_report_2 = [
        lambda db: db['EM4'] * 10,              # voltage akku 2
        lambda db: db['EM5'] * 10 / 2,          # current akku 2
        lambda db: (db['AS1'] + 1) * 100,       # trust
        lambda db: (db['AS2'] + 1) * 100,       # rudder
        lambda db: 0,
        lambda db: (int(db['SB1'])) & 0x0F      # 2nd nibble from SB1
    ]

    calculations_report_3 = [
        lambda db: db['EM3'],                   # capacity akku 1
        lambda db: db['EM6'],                   # # capacity akku 1
        lambda db: 0,
        lambda db: 0,
        lambda db: 0,
        lambda db: 0
    ]
    
    
    # Position Report (Frame 1)
    velocity = shared_datablock.data["SB5"]
    lat = shared_datablock.data["SB3"]
    long = shared_datablock.data["SB2"]
    bearing = shared_datablock.data["SB6"]
    velocity_kn = velocity * 1.94384
    aprs_msg_1 = position_report(lat, long, bearing, velocity_kn)       

     
    # Telemetry Report 1 (Frame 2)
    report_1 = [calc(shared_datablock.data) for calc in calculations_report_1]
    a1, a2, a3, a4, a5, d1  = report_1
    d1 = create_byte(d1, 1) 
    t_paket += 1
    aprs_msg_2 = telemetry_report(t_paket, a1, a2, a3, a4, a5, d1)

    
    # Telemetry Report 2 (Frame 3)
    report_2 = [calc(shared_datablock.data) for calc in calculations_report_2]
    a1, a2, a3, a4, a5, d1  = report_2
    d1 = create_byte(d1, 2) 
    t_paket += 1
    aprs_msg_3 = telemetry_report(t_paket, a1, a2, a3, a4, a5, d1)
 
    
    # Telemetry Report 3(Frame 4)
    report_3 = [calc(shared_datablock.data) for calc in calculations_report_3]
    a1, a2, a3, a4, a5, d1  = report_3
    d1 = create_byte(9, 3)
    t_paket += 1
    aprs_msg_4 = telemetry_report(t_paket, a1, a2, a3, a4, a5, d1)

    return aprs_msg_1, aprs_msg_2, aprs_msg_3, aprs_msg_4    


# main
if __name__ == "__main__":    
    t_paket = 0
    aprs_messages = get_data(t_paket)
    for i, frm in enumerate(aprs_messages, start=0):
        print(f"frm_{i}: {frm}")
 