# -*- coding: utf-8 -*-
"""
Created on Wed Jan 31 15:12:57 2024

Program history
21.01.2024    V. 00.01    start 
14.07.2024    V. 00.02    Datablock tu MQTT
16.07.2024    V. 00.03    send SB5 to MQTT in km/h

@author: Prof. Grabow (grabow@amesys.de)
"""
__version__ = '00.03'
__author__ = 'Joe Grabow'

import json
import paho.mqtt.client as mqtt
from time import sleep
import shared_datablock  # data from bus

# MQTT Dictionary
mqttdata = {
    "SB1": {
        "Topic": "USV/Status/Errorbyte",
        "value": "0"  
    },
    "SB2": {
        "Topic": "USV/Position/Longitude",
        "value": "0"  
    },
    "SB3": {
        "Topic": "USV/Position/Latitude",
        "value": "0"  
    },
    "GPS": {
        "Topic": "USV/Position/GPS",
        "value": "0.0#0.0"  
    },
    "SB5": {
        "Topic": "USV/Position/Geschwindigkeit",
        "value": "0.0"  
    },
    "SB6": {
        "Topic": "USV/Position/Kurswinkel",
        "value": "0.0"  
    },
    "AS1": {
        "Topic": "USV/Antrieb/Schub",
        "value": "0.0"  
    },
    "AS2": {
        "Topic": "USV/Antrieb/Ruder",
        "value": "0.0"  
    },
    "EM1": {
        "Topic": "USV/Energie/Akku1/Spannung",
        "value": "0.0"  
    },
    "EM2": {
        "Topic": "USV/Energie/Akku1/Strom",
        "value": "0.0"  
    },
    "EM3": {
        "Topic": "USV/Energie/Akku1/Kapazitaet",
        "value": "0.0" 
    },
    "EM4": {
        "Topic": "USV/Energie/Akku2/Spannung",
        "value": "0.0"  
    },
    "EM5": {
        "Topic": "USV/Energie/Akku2/Strom",
        "value": "0.0" 
    },
    "EM6": {
        "Topic": "USV/Energie/Akku2/Kapazitaet",
        "value": "0.0"  
    },
    "EM7": {
        "Topic": "USV/Energie/Solar/Spannung",
        "value": "0.0"  
    },
    "EM8": {
        "Topic": "USV/System/Temperatur",
        "value": "0.0" 
    },
    "EM9": {
        "Topic": "USV/Energie/Lidar/Spannung",
        "value": "0.0"  
    }
}


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Verbindung erfolgreich hergestellt")
        # (Multi-level-Wildcard) 
        # client.subscribe("USV/#") only for receive messages
    else:
        print(f"Verbindung fehlgeschlagen, Code: {rc}")

def read_config():
    with open("mqtt.json", "r") as config_file:
        config_data = json.load(config_file)
    return config_data

# Callback-Funktion für den Empfang von Nachrichten
def on_message(client, userdata, msg):
    print(f"Empfangene Nachricht auf Topic {msg.topic}: {msg.payload.decode()}")

def get_data():
    
    # read shared_data und wirte mqtt_data
    keys = ["SB1", "SB2", "SB3", "SB6", "AS1", "AS2", "EM1", "EM2", "EM3", "EM4", "EM5", "EM6", "EM7", "EM8", "EM9"]
    for key in keys:
        mqttdata[key]["value"] = str(shared_datablock.data[key])

    # only for Speed, convert from m/s in km/h
    mqttdata["SB5"]["value"] = f"{str(shared_datablock.data['SB5'] * 3.6)}"

    # only for GPS
    mqttdata["GPS"]["value"] = f"{str(shared_datablock.data['SB3'])}#{str(shared_datablock.data['SB2'])}"
    
    config = read_config()  # MQTT config
    client = mqtt.Client()  # init MQTT-Client
    client.on_connect = on_connect

    # connect to MQTT-Broker
    client.username_pw_set(config["mqtt"]["username"], config["mqtt"]["password"])
    client.connect(config["mqtt"]["broker"], config["mqtt"]["port"], 60)

    # send Topics
    for key, data in mqttdata.items():
        topic = data["Topic"]
        value = data["value"]
        print(f"Value {value} to MQTT Topic {topic}")
        client.publish(topic, value)
        sleep(0.1)

    return

if __name__ == "__main__":
    get_data()