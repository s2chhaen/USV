# -*- coding: utf-8 -*-
"""
Created on Thu Jul 11 12:59:51 2024

Program history
11.07.2024    V. 00.01    start

GPS Simulator
The program dz_gps.py reads a planned route from a csv table and determines the 
current coordinates, current speed, and course angle with each call to the 
function get_data(delay). These data are then entered into the shared 
data block of the bus system. The variable delay is the time between two calls 
to the get_data(delay) function.

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.01'
__author__ = 'Joe Grabow'

import shared_datablock
import csv
from nautic import get_distance, get_bearing

TABLE_NAME = "positions.csv"  # simulations positions data 


def read_position_table(table_name):
    """ GPS position from *.csv table """
    with open(table_name, 'r') as csv_data:
        csv_reader = csv.reader(csv_data, delimiter='\t')
        csv_lat = []
        csv_long = []
        for row in csv_reader:
            csv_lat.append(row[0])
            csv_long.append(row[1])
    return csv_lat, csv_long


def aprs_data_lat(aprs_data):
    """ latitude generator """
    iterator = iter(aprs_data)
    try:
        prev_entry = next(iterator)  # get the first entry
        while True:
            current_entry = next(iterator)  # get the current entry
            yield prev_entry, current_entry
            prev_entry = current_entry  # current element becomes the previous element for the next iteration
    except StopIteration:
        pass
    
 
def aprs_data_long(aprs_data):
    """ longitude generator """
    iterator = iter(aprs_data)
    try:
        prev_entry = next(iterator)
        while True:
            current_entry = next(iterator)
            yield prev_entry, current_entry
            prev_entry = current_entry
    except StopIteration:
        pass       


def get_data(delay):
    """ determines gps data between a time range """ 
    lat1, lat2 = next(aprs_lat, None)  # get current and next latitude
    long1, long2 = next(aprs_long, None)  # get current and next longitude
      
    if lat2 is not None:
        distance = get_distance(lat1, long1, lat2, long2) * 1000  # distance in Meter
        bearing = get_bearing(lat1, long1, lat2, long2)  # bearing in degree
        velocity = distance / delay  # velocity in m/s
        shared_datablock.data["SB5"] = velocity  # velocity  
        shared_datablock.data["SB6"] = bearing  # bearing   
    
    shared_datablock.data["SB2"] = long1  # longitude
    shared_datablock.data["SB3"] = lat1  # latitude   
    shared_datablock.data["SB4"] = 11  # Satfix (fictitious value)   


# main (run first)
csv_lat, csv_long = read_position_table(TABLE_NAME)  # read table
aprs_lat = aprs_data_lat(csv_lat)  # create Generator 1
aprs_long = aprs_data_long(csv_long)  # create Generator 2
