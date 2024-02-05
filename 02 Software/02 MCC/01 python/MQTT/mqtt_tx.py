# -*- coding: utf-8 -*-
"""
Created on Wed Jan 31 15:12:57 2024

@author: Grabow
"""
import json
import paho.mqtt.client as mqtt
from time import sleep

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Verbindung erfolgreich hergestellt")
        # Topic abonnieren (Multi-level-Wildcard) 
        client.subscribe("USV/#")
    else:
        print(f"Verbindung fehlgeschlagen, Code: {rc}")

def read_config():
    with open("mqtt_tx.json", "r") as config_file:
        config_data = json.load(config_file)
    return config_data

# Callback-Funktion für den Empfang von Nachrichten
def on_message(client, userdata, msg):
    print(f"Empfangene Nachricht auf Topic {msg.topic}: {msg.payload.decode()}")

def main():
    # Lese die Konfigurationsdaten
    config = read_config()

    # MQTT-Client initialisieren
    client = mqtt.Client()
    client.on_connect = on_connect
    #client.on_message = on_message


    # MQTT-Broker-Verbindung herstellen
    client.username_pw_set(config["mqtt"]["username"], config["mqtt"]["password"])
    client.connect(config["mqtt"]["broker"], config["mqtt"]["port"], 60)

    # Loop starten, um auf Nachrichten zu warten
    #client.loop_start()

    # Werte an Topics senden
    for topic, value in config["topics"].items():
        print(f"Sende Wert {value} an Topic {topic}")
        client.publish(topic, value)
        sleep(3)  # Kurze Pause zwischen den Veröffentlichungen

    # Programm in einer Endlosschleife halten, um die Verbindung aufrechtzuerhalten
    #client.loop_forever()
    return

if __name__ == "__main__":
    main()
