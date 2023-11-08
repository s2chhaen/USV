# -*- coding: utf-8 -*-
"""
Created on Wed Nov  8 13:22:12 2023

Program history
08.11.2023    V. 1.0    Start

@author: Prof. Jörg Grabow (grabow@amesys.de)
"""
__version__ = '1.0'
__author__ = 'Joe Grabow'

import matplotlib.pyplot as plt
import pandas as pd

# CSV-Datei laden
data = pd.read_csv('data.csv', header=None)

# Extrahieren der Spalten x, y und z
x = data[0]
y = data[1]
z = data[2]

# Diagramm xy
plt.figure(figsize=(8, 6))
plt.scatter(x, y, color='blue', label='xy')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.title('Diagramm xy')
plt.show()

# Diagramm xz
plt.figure(figsize=(8, 6))
plt.scatter(x, z, color='red', label='xz')
plt.xlabel('x')
plt.ylabel('z')
plt.legend()
plt.title('Diagramm xz')
plt.show()

# Diagramm yz
plt.figure(figsize=(8, 6))
plt.scatter(y, z, color='green', label='yz')
plt.xlabel('y')
plt.ylabel('z')
plt.legend()
plt.title('Diagramm yz')
plt.show()

# Hard iron distortion
offset_x = (max(x) + min(x)) / 2
offset_y = (max(y) + min(y)) / 2
offset_z = (max(z) + min(z)) / 2

corrected_x = x - offset_x
corrected_y = y - offset_y
corrected_z = z - offset_z

print("offset x: ", offset_x)
print("offset y: ", offset_y)
print("offset z: ", offset_z)

# Soft iron distortion
avg_delta_x = (max(x) - min(x)) / 2
avg_delta_y = (max(y) - min(y)) / 2
avg_delta_z = (max(z) - min(z)) / 2

avg_delta = (avg_delta_x + avg_delta_y + avg_delta_z) / 3

scale_x = avg_delta / avg_delta_x
scale_y = avg_delta / avg_delta_y
scale_z = avg_delta / avg_delta_z

print("scale x: ", scale_x)
print("scale y: ", scale_y)
print("scale z: ", scale_z)

corrected_x = (x - offset_x) * scale_x
corrected_y = (y - offset_y) * scale_y
corrected_z = (z - offset_z) * scale_z

# Diagramm scale xy
plt.figure(figsize=(8, 8))
plt.scatter(corrected_x, corrected_y, color='blue', label='xy')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.title('Diagramm scale xy')
plt.show()

# Diagramm scale xz
plt.figure(figsize=(8, 8))
plt.scatter(corrected_x, corrected_z, color='red', label='xz')
plt.xlabel('x')
plt.ylabel('z')
plt.legend()
plt.title('Diagramm scale xz')
plt.show()

# Diagramm scale yz
plt.figure(figsize=(8, 8))
plt.scatter(corrected_y, corrected_z, color='green', label='yz')
plt.xlabel('y')
plt.ylabel('z')
plt.legend()
plt.title('Diagramm scale yz')
plt.show()

