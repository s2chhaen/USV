# -*- coding: utf-8 -*-
"""
Created on Wed Jul 10 10:40:17 2024

Program history
10.07.2024    V. 00.01    Start (aprs_config.py)

you can generate the password for a valid call sign heree: 
https://apps.magicbug.co.uk/passcode/

@author: Prof. Grabow (grabow@amesys.de)
"""
import json
import base64

# Daten
data = {
    "ServerHost": "euro.aprs2.net",
    "ServerPort": 14580,
    "Callsign": "DL3AKB",
    "SSID": "-8",
    "Password": base64.b64encode("20397".encode()).decode('utf-8'),
    "Latitude": "5039.91N",
    "Longitude": "01107.22E",
    "Comment": "USV Nordwind",
    "Status": "on trip",
    "Status_Packet": False,  # False = Do not send status message packet
    "Primary_Symbol_Key": "/",
    "Secondary_Symbol_Key": "s",  # for a ship 
    "Delay": 30,
}

# Speichern der Daten in eine config.json Datei
with open('aprs_config.json', 'w') as config_file:
    json.dump(data, config_file, indent=4)
