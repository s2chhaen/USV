# -*- coding: utf-8 -*-
"""
Created on Wed Oct 16 16:47:39 2024

@author: Grabow
"""
import numpy as np
import matplotlib.pyplot as plt

# Hysterese
hysterese = 0.1

# Funktion Drive_A(Rudder, Trust)
def Drive_A(Rudder, Trust):
    if abs(Trust) <= hysterese:  # Trust ist nahe 0
        return -Rudder
    elif Rudder > 0:
        return (-2*Rudder + 1)*Trust
    else:
        return Trust

# Funktion Drive_B(Rudder, Trust)
def Drive_B(Rudder, Trust):
    if abs(Trust) <= hysterese:  # Trust ist nahe 0
        return Rudder
    elif Rudder < 0:
        return (2*Rudder + 1)*Trust
    else:
        return Trust

# Auswahl von festen Trust- und Rudder-Werten für die Liniendiagramme
fixed_Trust_values = [-1, -0.5, 0, 0.5, 1]  # Feste Trust-Werte
fixed_Rudder_values = [-1, -0.5, 0, 0.5, 1]  # Feste Rudder-Werte

# Erstellen der Rudder- und Trust-Werte für die Graphen
Rudder_values_graph = np.linspace(-1, 1, 400)
Trust_values_graph = np.linspace(-1, 1, 400)

# Plot für Drive_A(Rudder, Trust) gegen Rudder bei festen Trust-Werten
plt.figure(figsize=(12, 10))

# Drive_A(Rudder, Trust) gegen Rudder
plt.subplot(2, 2, 1)
for Trust_fixed in fixed_Trust_values:
    Drive_A_graph = [Drive_A(Rudder, Trust_fixed) for Rudder in Rudder_values_graph]
    plt.plot(Rudder_values_graph, Drive_A_graph, label=f'Trust = {Trust_fixed}')
plt.title('Drive_A(Rudder, Trust) über Rudder bei festen Trust-Werten')
plt.xlabel('Rudder')
plt.ylabel('Drive_A(Rudder, Trust)')
plt.legend()

# Drive_A(Rudder, Trust) gegen Trust bei festen Rudder-Werten
plt.subplot(2, 2, 2)
for Rudder_fixed in fixed_Rudder_values:
    Drive_A_graph = [Drive_A(Rudder_fixed, Trust) for Trust in Trust_values_graph]
    plt.plot(Trust_values_graph, Drive_A_graph, label=f'Rudder = {Rudder_fixed}')
plt.title('Drive_A(Rudder, Trust) über Trust bei festen Rudder-Werten')
plt.xlabel('Trust')
plt.ylabel('Drive_A(Rudder, Trust)')
plt.legend()

# Drive_B(Rudder, Trust) gegen Rudder bei festen Trust-Werten
plt.subplot(2, 2, 3)
for Trust_fixed in fixed_Trust_values:
    Drive_B_graph = [Drive_B(Rudder, Trust_fixed) for Rudder in Rudder_values_graph]
    plt.plot(Rudder_values_graph, Drive_B_graph, label=f'Trust = {Trust_fixed}')
plt.title('Drive_B(Rudder, Trust) über Rudder bei festen Trust-Werten')
plt.xlabel('Rudder')
plt.ylabel('Drive_B(Rudder, Trust)')
plt.legend()

# Drive_B(Rudder, Trust) gegen Trust bei festen Rudder-Werten
plt.subplot(2, 2, 4)
for Rudder_fixed in fixed_Rudder_values:
    Drive_B_graph = [Drive_B(Rudder_fixed, Trust) for Trust in Trust_values_graph]
    plt.plot(Trust_values_graph, Drive_B_graph, label=f'Rudder = {Rudder_fixed}')
plt.title('Drive_B(Rudder, Trust) über Trust bei festen Rudder-Werten')
plt.xlabel('Trust')
plt.ylabel('Drive_B(Rudder, Trust)')
plt.legend()

plt.tight_layout()
plt.show()
