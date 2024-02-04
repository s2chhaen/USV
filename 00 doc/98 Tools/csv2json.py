# -*- coding: utf-8 -*-
"""
Created on Sun Nov  5 18:03:42 2023

@author: Grabow
"""
import csv
import json

# Pfad zur CSV-Datei
csv_file = 'Datenblock.csv'

# Pfad zur JSON-Datei, in die umgewandelt werden soll
json_file = 'Datenblock.json'

# Öffne die CSV-Datei und lese sie ein
with open(csv_file, 'r') as csv_file:
    # Verwende csv.DictReader um die CSV-Datei einzulesen
    csv_data = csv.DictReader(csv_file)
    # Konvertiere die Daten in eine Liste von Dictionaries
    data = list(csv_data)

# Schreibe die Daten in eine JSON-Datei
with open(json_file, 'w') as json_file:
    json_file.write(json.dumps(data, indent=4))
