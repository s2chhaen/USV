# -*- coding: utf-8 -*-
"""
Created on Thu Nov 14 11:07:02 2024

@author: Grabow
"""
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Parameter
dt = 1  # Zeitschritt
r = 10    # Radius für Winkelbeschleunigungstransformation

# Kalman-Filter-Matrizen
A = np.array([[1, 0, dt, 0, 0.5 * dt**2, 0],
              [0, 1, 0, dt, 0, 0.5 * dt**2],
              [0, 0, 1, 0, dt, 0],
              [0, 0, 0, 1, 0, dt],
              [0, 0, 0, 0, 1, 0],
              [0, 0, 0, 0, 0, 1]])

H = np.array([[1, 0, 0, 0, 0, 0],  # GPS x
              [0, 1, 0, 0, 0, 0],  # GPS y
              [0, 0, 0, 0, 1, 0],  # Beschleunigung ax
              [0, 0, 0, 0, 0, 1]]) # Beschleunigung ay

Q = np.eye(6) * 0.01               # Prozessrauschen
R = np.diag([5, 5, 0.5, 0.5])      # Messrauschen für GPS und Beschleunigung

# Initialisierung
x = np.array([0, 0, 1, 1, 0, 0])   # Startzustand [x, y, vx, vy, ax, ay]
P = np.eye(6)                      # Anfangskovarianzmatrix

# Listen für die spätere Visualisierung
x_true = []
x_gps_noisy = []
x_acc_noisy = []
x_estimated = []

# CSV-Datei laden
data = pd.read_csv('sensor_data_0.csv', sep=';')

# Schleife über die Messdaten
for i in range(len(data)):
    time = data['time'][i]
    gps_x = data['gps_x'][i]
    gps_y = data['gps_y'][i]
    acc_x_measured = data['acc_x'][i]
    acc_y_measured = data['acc_y'][i]
    alpha = data['alpha'][i]  # Winkelbeschleunigung in rad/s²

    # GPS-Daten
    gps_noisy = np.array([gps_x, gps_y])
    x_gps_noisy.append(gps_noisy)

    # Transformation der Winkelbeschleunigung in lineare Beschleunigung
    theta = time  # Aktueller Winkel (einfache Annahme, abhängig von t)
    a_x_tangential = -r * alpha * np.sin(theta)
    a_y_tangential = r * alpha * np.cos(theta)

    # Zentripetalbeschleunigung (falls Winkelgeschwindigkeit omega bekannt wäre)
    omega = alpha * time  # Winkelgeschwindigkeit
    a_x_centripetal = -r * omega**2 * np.cos(theta)
    a_y_centripetal = -r * omega**2 * np.sin(theta)

    # Gesamte Beschleunigung durch Winkelbewegung
    a_x_winkel = a_x_tangential + a_x_centripetal
    a_y_winkel = a_y_tangential + a_y_centripetal
    
    """
    # Direkte Beschleunigungsmessung plus Rauschen
    acc_noisy_x = acc_x_measured + np.random.normal(0, 0.1)
    acc_noisy_y = acc_y_measured + np.random.normal(0, 0.1)
    """    
    
    # Direkte Beschleunigungsmessung ohne Rauschen
    acc_noisy_x = acc_x_measured
    acc_noisy_y = acc_y_measured

    # Gesamte Beschleunigung (gemessen + Winkelbeschleunigung)
    a_x_total = a_x_winkel + acc_noisy_x
    a_y_total = a_y_winkel + acc_noisy_y

    # Speichern der simulierten gemischten Beschleunigungen
    acc_combined_noisy = np.array([a_x_total, a_y_total])
    x_acc_noisy.append(acc_combined_noisy)

    # Kalman-Filter-Messvektor
    z = np.concatenate((gps_noisy, acc_combined_noisy))

    # Kalman-Filter Vorhersage
    x_pred = A @ x
    P_pred = A @ P @ A.T + Q

    # Kalman-Filter-Update
    y = z - H @ x_pred                # Innovationsvektor
    S = H @ P_pred @ H.T + R          # Innovationskovarianz
    K = P_pred @ H.T @ np.linalg.inv(S)  # Kalman-Gain

    x = x_pred + K @ y                # Aktualisierter Zustand
    P = (np.eye(6) - K @ H) @ P_pred  # Aktualisierte Kovarianzmatrix

    # Ergebnisse speichern
    x_estimated.append(x[:2])

# Listen in NumPy-Arrays umwandeln
x_gps_noisy = np.array(x_gps_noisy)
x_acc_noisy = np.array(x_acc_noisy)
x_estimated = np.array(x_estimated)

# Grafische Darstellung
plt.figure(figsize=(10, 8))
plt.plot(x_estimated[:, 0], x_estimated[:, 1], label="Kalman-gefilterte Position", color='green')
plt.scatter(x_gps_noisy[:, 0], x_gps_noisy[:, 1], label="GPS-Daten (gestört)", color='red', s=10)
plt.xlabel("x-Position")
plt.ylabel("y-Position")
plt.legend()
plt.title("Bewegung in der xy-Ebene: Kalman-gefilterte Positionen mit Winkel- und Linearbeschleunigung")
plt.grid()
plt.show()
