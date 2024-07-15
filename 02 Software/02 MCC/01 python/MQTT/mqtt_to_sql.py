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
06.02.2024    V. 1.3    Ausnahme für GPS-Koordinaten
15.07.2024    V. 1.4    Tabelle angepaßt, Logging hinzugefügt

@author: Prof. Jörg Grabow (grabow@amesys.de)
"""

__version__ = '1.4'
__author__ = 'Joe Grabow'


import json
import base64
import paho.mqtt.client as mqtt
import mysql.connector
import time
import atexit
import logging


# logging
logfile = 'mqtt_to_sql.log'

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    filename=logfile,  
    filemode='w'   
)

# SQL-Server
def connect_to_database():
    try:
        conn = mysql.connector.connect(
            host=config["sql"]["host"],
            user=base64.b64decode(config["sql"]["username"]).decode('utf-8'),
            password=base64.b64decode(config["sql"]["password"]).decode('utf-8'),
            database=config["sql"]["database"]
        )
        logging.info('SQL Datenbank verbunden')
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
        logging.info('SQL Datenbank geschlossen')
        logging.info('MQTT-Broker getrennt')


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logging.info('MQTT-Broker verbunden')
        # Topic abonnieren (Multi-level-Wildcard) 
        client.subscribe(config["topics"]["main"])
    else:
        print(f"Verbindung fehlgeschlagen, Code: {rc}")

def read_config():
    with open("mqtt_to_sql.json", "r") as config_file:
        config_data = json.load(config_file)
    return config_data


# Callback-Funktion für den Empfang von MQTT Nachrichten
def on_message(client, userdata, msg):
    global config
    
    #print(f"Nachricht empfangen: {msg.payload.decode()} von Topic: {msg.topic}")
    sql_table = config["Table"].get(msg.topic)  # Topic der SQL Tabelle zuordnen
    #sql_table = config["Table"][msg.topic]  # Topic der SQL Tabelle zuordnen
    if not sql_table:
        print(f"Keine Zuordnung für Topic '{msg.topic}' gefunden.")
        return

    try:
        if sql_table == "gps":  # Ausnahmebehandlung für GPS-Koordinaten
            gps_position = msg.payload.decode()
            latitude, longitude = map(float, gps_position.split("#"))
            cursor.execute("INSERT INTO {} (Latitude, Longitude) VALUES (%s, %s)".format(sql_table), (latitude, longitude))
        else:  # alle restlichen Topics
            sql_payload = msg.payload.decode()  # Payload des Topic
            cursor.execute("INSERT INTO {} (Messung) VALUES (%s)".format(sql_table), (sql_payload,))

        conn.commit()
        logging.info('SQL Daten geschrieben')
        #print("Erfolgreich eingefügt.")
    except Exception as e:
        print(f"Fehler beim Einfügen: {e}")

    return


#def main():
global config

print('MQTT to SQL gestartet')

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
# client.loop_forever()
client.loop_start()  # Starte den Netzwerk-Loop im Hintergrund
    
# MQTT-Nachrichten empfangen
try:
    while True:
        time.sleep(0.1)  # kurze Pause, um Nachricht zu empfangen
except KeyboardInterrupt:
    client.loop_stop()
    atexit.register(close_connections)





