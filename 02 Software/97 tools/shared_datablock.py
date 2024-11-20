# -*- coding: utf-8 -*-
"""
Created on Thu Jul 11 12:37:56 2024

Program history
11.07.2024    V. 00.01    start
20.11.2024    V. 00.02    Lichterführung

USV Datenblock

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.02'
__author__ = 'Joe Grabow'

import csv
"""
data = {
    "SB1": 0, # Error Status global
    "SB2": 0, # Längengrad (Fusion)
    "SB3": 0, # Breitengrad (Fusion)
    "SB4": 0, # SatFix (deprecate)
    "SB5": 0, # Geschwindigkeit (Fusion)
    "SB6": 0, # Kurswinkel (Fusion)
    "SB7": 0, # Timestamp
    "SB8": 0, # Radar Entfernung
    "SB9": 0, # Radar Geschwindigkeit
    "SB20": 0, # Längengrad
    "SB21": 0, # Breitengrad
    "SB22": 0, # SatFix
    "SB23": 0, # GPS Geschwindigkeit
    "SB24": 0, # Kurswinkel Kompass
    "SB25": 0, # IMU Beschleunigung x
    "SB26": 0, # IMU Beschleunigung y
    "SB27": 0, # IMU Beschleunigung z
    "SB28": 0, # IMU Omega x
    "SB29": 0, # IMU Omega y
    "SB30": 0, # IMU Omega z
    "SB31": 0, # Rollwinkel um x
    "SB32": 0, # Nickwinkel um y
    "SB33": 0, # Gierwinkel um z
    "AF1": 0, # Führung Punkt A, Länge, Breite
    "AF2": 0, # Führung Punkt B, Länge, Breite
    "AF3": 0, # Führung Geschwindigkeit
    "AF4": 0, # Führung Epsilon
    "AS1": 0, # Schub
    "AS2": 0, # Ruder
    "EM1": 0, # Spannung Akku 1
    "EM2": 0, # Strom Akku 1
    "EM3": 0, # Ladung Akku 1
    "EM4": 0, # Spannung Akku 2
    "EM5": 0, # Strom Akku 2
    "EM6": 0, # Ladung Akku 2
    "EM7": 0, # Ladespannung Solar
    "EM8": 0, # Board-Temperatur
    "EM9": 0, # Spannung Lidar
    "LF1": 0, # Licht ON/OFF
    "ER1": 0, # ESB GPS
    "ER2": 0, # ESB Kompass
    "ER3": 0, # ESB Control
    "ER4": 0, # ESB Lidar
    "ER5": 0, # ESB Radar
    "ER6": 0, # ESB IMU
    "ER7": 0, # ESB Antrieb
    "ER8": 0, # ESB Solar
    "ER9": 0, # ESB APRS
    "ER10": 0, # ESB Licht
    "ES1": 0 # Lidar Entfernung
}
"""

def read_csv_to_dict(file_path):
    """ read datablock from Datenblock.csv"""
    data_dict = {}

    with open(file_path, 'r', newline='') as csvfile:
        csv_reader = csv.reader(csvfile, delimiter=';')
        
        # Überspringe die erste Zeile
        next(csv_reader, None)
        
        # Annahme: Die Datei hat mindestens 5 Spalten
        for row in csv_reader:
            if len(row) >= 5:
                key = row[2]  # Dritte Spalte als Key         
                value = float(row[4].replace(',', '.'))  # Dezimaltrennzeichen ersetzen
                data_dict[key] = value

    return data_dict

file_path = 'Datenblock.csv'  # name and path of the datablock  
data = read_csv_to_dict(file_path)  # read data in a dictionary
