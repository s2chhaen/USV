# -*- coding: utf-8 -*-
"""
Created on Wed Jan 31 16:12:37 2024

Das Tool abonniert einen Topic vom MQTT-Broker und speichert die empfangenen 
Daten in einer SQL-Datenbank.

Alle Konfigurationseinstellungen sind in der Datei "mqtt_to_sql.json"
gespeichert.

Unter "topics" ist der abonnierte "Maintopic" gespeichert, 
Wildcards sind möglich.

"Table" beinhaltet die Zuordnung des jeweiligen Topic zur Datenbanktabelle.
Alle Messwerte werden in der jeweiligen Tabelle immer in der Spalte
"Messung" gespeichert. 


Program history
01.02.2024    V. 1.0    Start
02.02.2024    V. 1.1    SQL-Injektionen verhindern
05.02.2024    V. 1.2    Fehlerbehandlung

@author: Prof. Jörg Grabow (grabow@amesys.de)
"""

__version__ = '1.2'
__author__ = 'Joe Grabow'


import json
import paho.mqtt.client as mqtt
import mysql.connector
import time
import atexit


# SQL-Server
def connect_to_database():
    try:
        conn = mysql.connector.connect(
            host=config["sql"]["host"],
            user=config["sql"]["username"],
            password=config["sql"]["password"],
            database=config["sql"]["database"]
        )
        print("Verbindung zur Datenbank erfolgreich hergestellt")
        return conn
    except mysql.connector.Error as e:
        print(f"Fehler bei der Verbindung zur Datenbank: {e}")
        return None


# MQTT-Broker
def connect_to_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(config["mqtt"]["username"], config["mqtt"]["password"])
    client.connect(config["mqtt"]["broker"], config["mqtt"]["port"], 60)
    return client


# close all connections
def close_connections():
    client.disconnect()
    if conn:
        conn.close()
        print("Datenbankverbindung geschlossen")


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Verbindung zum MQTT-Broker erfolgreich hergestellt")
        # Topic abonnieren (Multi-level-Wildcard) 
        client.subscribe(config["topics"]["main"])
    else:
        print(f"Verbindung fehlgeschlagen, Code: {rc}")

def read_config():
    with open("mqtt_to_sql_usb.json", "r") as config_file:
        config_data = json.load(config_file)
    return config_data


# Callback-Funktion für den Empfang von Nachrichten
def on_message(client, userdata, msg):
    sql_table = config["Table"][msg.topic]  # Topic der SQL Tabelle zuordnen
    #print(msg.topic)
    #print(format(sql_table))
    sql_payload = msg.payload.decode()  # Payload des Topic
    #print(sql_payload)
    try:
        cursor.execute("INSERT INTO {} (Messung) VALUES (%s)".format(sql_table), (sql_payload,))
        conn.commit()
        print("Erfolgreich eingefügt.")
    except Exception as e:
        print(f"Fehler beim Einfügen: {e}")
    return

#def main():
global config

# Lese die Konfigurationsdaten
config = read_config()

# Verbindung zur Datenbank herstellen
conn = connect_to_database()
cursor = conn.cursor()  # Ein Cursor-Objekt erstellen

# MQTT-Client initialisieren
client = connect_to_mqtt()
client.on_connect = on_connect
client.on_message = on_message

# Loop starten, um auf Nachrichten zu warten
client.loop_start()
    
# MQTT-Nachrichten empfangen
try:
    while True:
        time.sleep(0.1)  # kurze Pause, um Nachricht zu empfangen
except KeyboardInterrupt:
    atexit.register(close_connections)





