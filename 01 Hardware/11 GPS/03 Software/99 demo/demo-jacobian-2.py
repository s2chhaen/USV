# -*- coding: utf-8 -*-
"""
Created on Fri Nov 22 06:26:02 2024

@author: R
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Parameter
dt = 1  # Zeitschritt
r = 10    # Radius für Winkelbeschleunigungstransformation

# Kalman-Filter-Matrizen
H = np.array([[1, 0, 0, 0, 0, 0],  # GPS x
              [0, 1, 0, 0, 0, 0],  # GPS y
              [0, 0, 0, 0, 1, 0],  # Beschleunigung ax
              [0, 0, 0, 0, 0, 1]]) # Beschleunigung ay

Q_base = np.eye(6) * 0.01          # Prozessrauschen
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
data = pd.read_csv('sensor_data_1.csv', sep=';')

def fx(state):
    """
    定义状态转移函数
    """
    dt = 1  # 时间步长
    return np.array([
        state[0] + state[2] * dt + 0.5 * state[4] * dt**2,
        state[1] + state[3] * dt + 0.5 * state[5] * dt**2,
        state[2] + state[4] * dt,
        state[3] + state[5] * dt,
        state[4],
        state[5]
    ])

def jacobian(f, x, epsilon=1e-5):
    """
    使用有限差分法计算雅可比矩阵（Jacobian Matrix）
    """
    x = np.asarray(x, dtype=float)
    n = len(x)  # 输入向量的维度
    f_x = np.asarray(f(x), dtype=float)
    m = len(f_x)  # 输出向量的维度

    # 初始化雅可比矩阵 (m x n)
    J = np.zeros((m, n))

    # 使用有限差分计算偏导数
    for i in range(n):
        x_perturbed = np.copy(x)
        x_perturbed[i] += epsilon  # 增加一个微小扰动
        f_perturbed = np.asarray(f(x_perturbed), dtype=float)
        J[:, i] = (f_perturbed - f_x) / epsilon  # 偏导数计算

    return J
    
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
    
    # 动态调整过程噪声（根据加速度变化率）
    acc_variation = np.abs(acc_noisy_x) + np.abs(acc_noisy_y)
    Q = Q_base * (1 + acc_variation)

    # Kalman-Filter-Messvektor
    z = np.concatenate((gps_noisy, acc_combined_noisy))
    
    JA = jacobian(lambda state: fx(state), x)
    # Kalman-Filter Vorhersage
    x_pred = fx(x)
    P_pred = JA @ P @ JA.T + Q

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