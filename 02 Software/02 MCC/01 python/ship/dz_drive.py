# -*- coding: utf-8 -*-
"""
Created on Fri Jul 12 10:42:14 2024

Program history
12.07.2024    V. 00.01    start

Drive Simulator
detailed calculation in document dz_drive.pdf

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.01'
__author__ = 'Joe Grabow'

import shared_datablock


P_M = 1920  # drive power in W (both engines)
ETA = 0.7  # efficiency
K_ST = 20  # Stokes friction in kg/m
U_1 = 23.1 # default voltage battery 1
U_2 = 23.1 # default voltage battery 2


def get_value(data_block, key, default):
    value = data_block.get(key, 0)
    return default if value == 0 else value


def get_data():
    u_1 = get_value(shared_datablock.data, "EM1", U_1)
    u_2 = get_value(shared_datablock.data, "EM4", U_2)
    v = shared_datablock.data["SB5"]

    # calculate current
    i_1 = (K_ST * v ** 3) / (2 * u_1 * ETA )
    i_2 = (K_ST * v ** 3) / (2 * u_2 * ETA )
    
    # calculate trust
    power_1 = i_1 * u_1
    power_2 = i_2 * u_2
    trust = (power_1 + power_2) / P_M  
    
    # save data
    shared_datablock.data["EM2"] = i_1  # current drive 1
    shared_datablock.data["EM5"] = i_2  # current drive 2
    shared_datablock.data["AS1"] = trust  # trust

# main
if __name__ == "__main__":
    get_data()