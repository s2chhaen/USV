# -*- coding: utf-8 -*-
"""
Created on Mon Nov 18 14:58:28 2024

@author: Grabow
"""
import json
import sys
import re
from xml.etree.ElementTree import Element, SubElement, tostring
from xml.dom.minidom import parseString

# Funktion zur Konvertierung von Grad-Minuten-Koordinaten in Dezimal
def dms_to_decimal(coord):
    match = re.match(r"(\d+)Â° (\d+\.\d+)' ([NSEW])", coord)
    if not match:
        raise ValueError(f"Ungültiges Koordinatenformat: {coord}")
    degrees, minutes, direction = match.groups()
    decimal = float(degrees) + float(minutes) / 60
    if direction in "SW":
        decimal = -decimal
    return decimal

# Pfad zur JSON-Datei
json_file_path = 'data_log.json'

# JSONL-Datei zeilenweise einlesen
coordinates = []
try:
    with open(json_file_path, 'r') as file:
        for line in file:
            try:
                data = json.loads(line)
                # Koordinaten extrahieren und umwandeln
                lat = dms_to_decimal(data['Latitude'])
                lon = dms_to_decimal(data['Longitude'])
                coordinates.append((lat, lon))
            except json.JSONDecodeError as e:
                print(f"Fehler beim Parsen einer Zeile: {e}")
            except ValueError as ve:
                print(f"Ungültige Koordinaten gefunden: {ve}")
except FileNotFoundError:
    print(f"Die Datei {json_file_path} wurde nicht gefunden.")
    sys.exit()

# Überprüfen, ob Daten vorhanden sind
if not coordinates:
    print("Keine gültigen Koordinaten gefunden.")
    sys.exit()

# GPX-Datei erstellen
gpx = Element("gpx", version="1.1", creator="Python GPX Generator", xmlns="http://www.topografix.com/GPX/1/1")
trk = SubElement(gpx, "trk")
trkseg = SubElement(trk, "trkseg")

for lat, lon in coordinates:
    trkpt = SubElement(trkseg, "trkpt", lat=str(lat), lon=str(lon))

# GPX-Daten formatieren und speichern
gpx_data = parseString(tostring(gpx)).toprettyxml(indent="  ")

gpx_file_path = "route.gpx"
with open(gpx_file_path, "w") as gpx_file:
    gpx_file.write(gpx_data)

print(f"GPX-Datei erfolgreich erstellt: {gpx_file_path}")
