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
16.07.2024    V. 1.5    Kommandozeilenparameter zum Logging

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
import argparse

# ArgumentParser erstellen
parser = argparse.ArgumentParser(description="Skript mit Logging.")
parser.add_argument('--log', action='store_true', help='Aktiviere Logging')
args = parser.parse_args()  # Argumente parsen

if args.log:
    # Logging
    logfile = 'mqtt_to_sql.log'
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(levelname)s - %(message)s',
        filename=logfile,
        filemode='w'
        )

# Konfiguration laden
def read_config():
    with open("mqtt_to_sql.json", "r") as config_file:
        config_data = json.load(config_file)
    return config_data

# SQL-Server Verbindung
def connect_to_database(config):
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
        logging.error(f"Fehler bei der Verbindung zur Datenbank: {e}")
        return None

# MQTT-Broker Verbindung
def connect_to_mqtt(config, on_connect, on_message):
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(config["mqtt"]["username"], config["mqtt"]["password"])
    client.connect(config["mqtt"]["broker"], config["mqtt"]["port"], 60)
    return client

# Verbindungen schließen
def close_connections(client, conn):
    if client.is_connected():
        client.disconnect()
        logging.info('MQTT-Broker getrennt')
    if conn:
        conn.close()
        logging.info('SQL Datenbank geschlossen')

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logging.info('MQTT-Broker verbunden')
        client.subscribe(config["topics"]["main"])
    else:
        logging.error(f"Verbindung fehlgeschlagen, Code: {rc}")

def on_message(client, userdata, msg):
    sql_table = config["Table"].get(msg.topic)
    if not sql_table:
        logging.warning(f"Keine Zuordnung für Topic '{msg.topic}' gefunden.")
        return

    try:
        payload = msg.payload.decode()
        if sql_table == "gps":
            latitude, longitude = map(float, payload.split("#"))
            cursor.execute(f"INSERT INTO {sql_table} (Latitude, Longitude) VALUES (%s, %s)", (latitude, longitude))
        else:
            cursor.execute(f"INSERT INTO {sql_table} (Messung) VALUES (%s)", (payload,))
        conn.commit()
        logging.info(f'Daten erfolgreich in Tabelle {sql_table} eingefügt')
    except Exception as e:
        logging.error(f"Fehler beim Einfügen: {e}")

# Main
if __name__ == "__main__":
    print('MQTT to SQL gestartet')
    logging.info('MQTT to SQL gestartet')

    config = read_config()
    conn = connect_to_database(config)
    if not conn:
        logging.error('Datenbankverbindung konnte nicht hergestellt werden. Programm wird beendet.')
        exit(1)

    cursor = conn.cursor()

    client = connect_to_mqtt(config, on_connect, on_message)
    client.loop_start()

    atexit.register(close_connections, client, conn)

    try:
        while True:
            time.sleep(0.1)
    except KeyboardInterrupt:
        client.loop_stop()
        logging.info('Programm durch Benutzer beendet')
