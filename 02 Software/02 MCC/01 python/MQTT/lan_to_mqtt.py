# -*- coding: utf-8 -*-
"""
Created on Wed Jan 31 16:12:37 2024

Das Tool empfängt einen Datenblock (Binärdaten) über WLAN/Lan vom USV,
konvertiert diese Daten in ein Float-Format und legt sie im Shared_Datablock
ab. Anschließend wird dieser Datenblock an einen MQTT-Broker gesendet.

Konfigurationseinstellungen für das Lan-Netzwerk in "lan.json"
Konfigurationseinstellungen für den MQTT-Broker in "mqtt.json"
gespeichert.


Program history
18.07.2024    V. 1.0    Start


@author: Prof. Jörg Grabow (grabow@amesys.de)
"""

__version__ = '1.0'
__author__ = 'Joe Grabow / Stefan Franke'

import json
import paho.mqtt.client as mqtt
import time
import atexit
import logging
import argparse
import shared_datablock
import dz_data2mqtt

# ArgumentParser erstellen
parser = argparse.ArgumentParser(description="Skript mit Logging.")
parser.add_argument('--log', action='store_true', help='Aktiviere Logging')
args = parser.parse_args()  # Argumente parsen

if args.log:
    # Logging
    logfile = 'lan_to_mqtt.log'
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(levelname)s - %(message)s',
        filename=logfile,
        filemode='w'
        )



# Main
if __name__ == "__main__":
    print('Startmeldung für dieses Tool')
    logging.info('Lan to MQTT gestartet')

    try:
        while True:
            """ Funktion muss noch gebaut werden
            lan.getdata()  # Lan-Binary-Data to shared_Datablock 
            """
            shared_datablock.getdata()  # send Datablock to MQTT
            time.sleep(0.1)
    except KeyboardInterrupt:

        logging.info('Programm durch Benutzer beendet')
