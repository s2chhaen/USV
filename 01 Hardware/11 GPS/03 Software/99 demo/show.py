# -*- coding: utf-8 -*-
"""
Created on Wed Nov 20 10:19:15 2024

@author: Grabow
"""
import json
import matplotlib.pyplot as plt
import sys
import re
import numpy as np


# Visualisierung
def plot_cartesian_coords(cartesian_coords, labels=None):
    """
    Plottet die kartesischen Koordinaten auf einer 2D-Karte.

    :param cartesian_coords: Liste von (x, y)-Koordinaten
    :param labels: Optionale Labels für die Punkte
    """
    x_coords, y_coords = zip(*cartesian_coords)
    plt.figure(figsize=(8, 6))
    plt.scatter(x_coords, y_coords, color='blue', marker='.', label="Koordinaten")

    """    
    # Optionale Beschriftung
    if labels:
        for i, label in enumerate(labels):
            plt.text(
                x_coords[i], y_coords[i], label, fontsize=10, ha='left', va='bottom'
            )
    """        
            
    plt.axhline(0, color='gray', linestyle='--', linewidth=0.5)
    plt.axvline(0, color='gray', linestyle='--', linewidth=0.5)
    plt.grid(color='lightgray', linestyle='--', linewidth=0.5)
    plt.title("Kartesische Koordinaten relativ zum Referenzpunkt")
    plt.xlabel("x (Ost-West, in Metern)")
    plt.ylabel("y (Nord-Süd, in Metern)")
    plt.axis("equal")
    plt.legend()
    plt.show()


# Konvertierung von geographischen Koordinaten
def geo_to_local_cartesian_from_list(latitude_list, longitude_list, radius=6371000):
    """
    Konvertiert eine Liste von geographischen Koordinaten in ein lokales kartesisches Koordinatensystem.
    Die erste Koordinate wird als Referenzpunkt verwendet.

    :param latitude_list: Liste von Breiten (in Dezimalgrad)
    :param longitude_list: Liste von Längen (in Dezimalgrad)
    :param radius: Erdradius in Metern (Standard: 6371000 m)
    :return: Liste von (x, y)-Koordinaten in Metern
    """
    if len(latitude_list) != len(longitude_list):
        raise ValueError("Die Listen latitude_list und longitude_list müssen gleich lang sein.")

    lat_ref = latitude_list[0]
    lon_ref = longitude_list[0]

    lat_ref_rad = np.radians(lat_ref)
    lon_ref_rad = np.radians(lon_ref)

    cartesian_coords = []

    for lat, lon in zip(latitude_list, longitude_list):
        lat_rad = np.radians(lat)
        lon_rad = np.radians(lon)

        x = radius * np.cos(lat_ref_rad) * (lon_rad - lon_ref_rad)
        y = radius * (lat_rad - lat_ref_rad)

        cartesian_coords.append((x, y))

    return cartesian_coords


# Funktion zur Umwandlung von DMS-Koordinaten in Dezimal
def dms_to_decimal(coord):
    match = re.match(r"(\d+)Â°\s*(\d+\.\d+)'?\s*([NSEW])", coord)
    if not match:
        raise ValueError(f"Ungültiges Koordinatenformat: {coord}")
    degrees, minutes, direction = match.groups()
    decimal = float(degrees) + float(minutes) / 60
    if direction in "SW":
        decimal = -decimal
    return decimal


# JSON-Dateipfad und Koordinatenlisten
json_file_path = "data_log.json"
latitude_list = []
longitude_list = []
labels = []

try:
    with open(json_file_path, "r") as file:
        for i, line in enumerate(file):
            try:
                data = json.loads(line)
                latitude = dms_to_decimal(data["Latitude"])
                longitude = dms_to_decimal(data["Longitude"])
                latitude_list.append(latitude)
                longitude_list.append(longitude)
                labels.append(f"Punkt {i}")  # Optionale Labels
            except (json.JSONDecodeError, ValueError) as e:
                print(f"Fehler beim Verarbeiten der Zeile {i + 1}: {e}")

except FileNotFoundError:
    print(f"Die Datei {json_file_path} wurde nicht gefunden.")
    sys.exit()

# Überprüfung auf Daten
if not latitude_list or not longitude_list:
    print("Keine gültigen Koordinaten gefunden.")
    sys.exit()

# Berechnung der kartesischen Koordinaten
cartesian_coords = geo_to_local_cartesian_from_list(latitude_list, longitude_list)

# Plot
plot_cartesian_coords(cartesian_coords, labels=labels)
