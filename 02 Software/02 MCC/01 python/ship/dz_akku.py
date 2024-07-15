# -*- coding: utf-8 -*-
"""
Created on Fri Jul 12 08:32:50 2024

Program history
12.07.2024    V. 00.01    start

Akku Simulator
Attention: During the first program run, no current exists yet. 
The dt_drive.py module must be started beforehand.

detailed calculation in document dz_akku.pdf

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.01'
__author__ = 'Joe Grabow'


import shared_datablock
import math

C_N1 = 55.0  # single battery 1 capacity in Ah
C_N2 = 52.0  # single battery 2 capacity in Ah

K_P = 1.09  # Peukert constant NiMH Akku
K_E = 5  # Lipo constant
N_Z = 7  # number of cells
U_MIN = 2.1  # minimum voltage in V
U_MAX = 3.3  # maximum voltage in V
U_SOLAR = 24.3  # fictional value
U_LIDAR = 23.8  # fictional value


def get_data(delay):
    current_1 = shared_datablock.data["EM2"]
    current_2 = shared_datablock.data["EM5"]  
    c_n1 = shared_datablock.data["EM3"]  # single battery 1 capacity in Ah
    c_n2 = shared_datablock.data["EM6"]  # single battery 2 capacity in Ah
    
    # calculate capacity
    dc_n1 = delay * current_1 ** K_P  # result in As
    dc_n2 = delay * current_2 ** K_P  # result in As
    c_r1 = c_n1 * 3600 - dc_n1  # Remaining battery 1 capacity in As
    c_r2 = c_n2 * 3600 - dc_n2  # Remaining battery 2 capacity in As
    c_n1 = c_r1 / 3600  # new battery 1 capacity in Ah
    c_n2 = c_r2 / 3600  # new battery 2 capacity in Ah
    c_q1 = c_r1 / ( C_N1 * 3600 )  # State of charge (SoC) battery 1 in %
    c_q2 = c_r2 / ( C_N2 * 3600 )  # State of charge (SoC) battery 2 in %
    
    # calculate voltage (empirical model)
    u_1 = ((U_MIN - U_MAX) / (math.exp(c_q1 * K_E) + 1) + U_MAX) * N_Z
    u_2 = ((U_MIN - U_MAX) / (math.exp(c_q2 * K_E) + 1) + U_MAX) * N_Z
    
    # save data
    shared_datablock.data["EM1"] = u_1  # voltage akku 1
    shared_datablock.data["EM4"] = u_2  # voltage akku 2
    shared_datablock.data["EM3"] = c_n1  # Remaining battery 1 capacity in Ah
    shared_datablock.data["EM6"] = c_n2  # Remaining battery 2 capacity in Ah
    shared_datablock.data["EM7"] = U_SOLAR
    shared_datablock.data["EM9"] = U_LIDAR

# main
if __name__ == "__main__":
    get_data(30)