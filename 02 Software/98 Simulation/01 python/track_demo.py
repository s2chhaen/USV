# -*- coding: utf-8 -*-
"""
Created on Sat Dec 23 16:44:20 2023

@author: Joe Grabow

Dynamic model of a generic ship with 3 DoF.
https://github.com/Joe-Grabow/USV/
"""
__version__ = '1.0'
__author__ = 'Joe Grabow'

import matplotlib.pyplot as plt
from tools.vessel import init_vessel, run_vessel, get_drive


# Parameters of the dynamic ship model
dynamic_model = {
    'a': 0.1,           # distance between engines and centreline in m
    'm_S': 100,         # shipp mass in kg
    'J_S': 10,          # moment of inertia about the centre of gravity in kg*m^2
    'F_D': 0,           # drift force (wind) in N
    'beta': 0,          # drift angle
    'v_St': 0,          # water flow velocity
    'gamma': 0,         # angel of flow
    'k_x': 40,          # friction constant in x-direction
    'k_y': 80,          # friction constant in y-direction
    'k_t': 5            # friction constant for rotation
}

# ship drive
drive = {
    'F_A': 0,           # power engine A in N (off)
    'F_B': 0,           # power engine B in N (off)    
}


initial_conditions = {
    'omega_0': 0,       # angular velocity of the ship, axis of rotation
    'phi_0': 0,         # angle of rotation
    'vx_0': 0,          # velocity in x-direction
    'vy_0': 0,          # velocity in y-direction
    'X_0': 1,           # start position X (Inertialsystem)
    'Y_0': 1            # start position Y (Inertialsystem)
}

omega_0 = initial_conditions['omega_0']
phi_0 = initial_conditions['phi_0']
vx_0 = initial_conditions['vx_0']
vy_0 = initial_conditions['vy_0']
X_0 = initial_conditions['X_0']
Y_0 = initial_conditions['Y_0']


# main
init_vessel(dynamic_model)

""" track section 1 """
trust = 10
rudder = 0
drive['F_A'], drive['F_B']  = get_drive(trust, rudder)  # trust and rudder to drive

start_point = [omega_0, phi_0, vx_0, vy_0, X_0, Y_0]
time_span = (0, 10)
steps = 10
end_point, X, Y, heading, v_vessel = run_vessel(start_point, time_span, steps, drive)

# Plot der Lösung
plt.plot(X, Y, label='track section 1')
plt.xlabel('x-coordinate')
plt.ylabel('y-coordinate')
plt.legend()
plt.grid(True, linestyle='--', linewidth=0.5, color='gray', alpha=0.7)
plt.show()


""" track section 2 """
trust = 10
rudder = 0.2
drive['F_A'], drive['F_B']  = get_drive(trust, rudder)  # trust and rudder to drive

start_point = end_point
time_span = (0, 90)
steps = 90
end_point, X, Y, heading, v_vessel = run_vessel(start_point, time_span, steps, drive)

# plot the path
plt.plot(X, Y, label='track section 2')
#plt.xlabel('x-coordinate')
#plt.ylabel('y-coordinate')
plt.legend()
plt.show()


""" track section 3 """
trust = 5
rudder = 0
drive['F_A'], drive['F_B']  = get_drive(trust, rudder)  # trust and rudder to drive

start_point = end_point
time_span = (0, 20)
steps = 20
end_point, X, Y, heading, v_vessel = run_vessel(start_point, time_span, steps, drive)

# plot the path
plt.plot(X, Y, label='track section 3')
#plt.xlabel('x-coordinate')
#plt.ylabel('y-coordinate')
plt.legend()
plt.show()


