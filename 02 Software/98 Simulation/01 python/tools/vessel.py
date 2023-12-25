# -*- coding: utf-8 -*-
"""
Created on Sat Dec 23 16:44:20 2023

@author: Joe Grabow

Dynamic model of a generic ship with 3 DoF.
https://github.com/Joe-Grabow/USV/
"""
__version__ = '1.0'
__author__ = 'Joe Grabow'

import numpy as np
from scipy.integrate import solve_ivp, cumulative_trapezoid


F_A = 0
F_B = 0
a = 0
m_S = 0
J_S = 0
F_D = 0
beta = 0
v_St = 0
gamma = 0
k_x = 0
k_y = 0
k_t = 0


# Dgl. system of the ship's dynamics
def system(t, variables):
    d_phi_dt, phi, vx, vy = variables
    d2_phi_dt = -(k_t/J_S) * d_phi_dt + (a/J_S) * (F_B - F_A)
    d_vx_dt = (1/m_S) * (F_A + F_B - k_x *(vx - v_St * np.cos(gamma - phi)) + F_D * np.cos(beta - phi))
    d_vy_dt = (1/m_S) * (F_D * np.sin(beta - phi) - k_y * (vy - v_St * np.sin(gamma - phi)))
    return [d2_phi_dt, d_phi_dt, d_vx_dt, d_vy_dt]


def radiant_to_degrees(angle_in_radiant):
    angle_in_degrees = np.degrees(angle_in_radiant)
    angle_in_degrees_wrapped = np.fmod(angle_in_degrees, 360.0)
    angle_in_degrees_wrapped = np.where(angle_in_degrees_wrapped < 0, angle_in_degrees_wrapped + 360.0, angle_in_degrees_wrapped)
    return angle_in_degrees_wrapped


# rudder function
def get_drive(trust, rudder):
    if trust == 0:
        A = -rudder
        B = rudder
    else:
        if rudder >= 0:
            A = (1 - 2 * rudder) * trust
            B = trust
        elif rudder < 0:
            A = trust
            B = (1 + 2 * rudder) * trust
        else:
            A = B = 0
    return A, B


# initialises the dynamic state
def init_vessel(dynamic_model):
    global a, m_S, J_S, F_D, beta, v_St, gamma, k_x, k_y, k_t 
    a = dynamic_model['a']
    m_S = dynamic_model['m_S']
    J_S = dynamic_model['J_S']
    F_D = dynamic_model['F_D']
    beta = dynamic_model['beta']
    v_St = dynamic_model['v_St']
    gamma = dynamic_model['gamma']
    k_x = dynamic_model['k_x']
    k_y = dynamic_model['k_y']
    k_t = dynamic_model['k_t']    
    return


# Trajectory for given time interval
def run_vessel(start_point, time_span, steps, drive):
    global F_A, F_B
    F_A = drive['F_A']
    F_B = drive['F_B']
    initial_conditions = start_point[:-2]  # intertial conditions for Dgl.-System
    time_evaluations = np.linspace(time_span[0], time_span[1], steps)
    solution = solve_ivp(system, time_span, initial_conditions, t_eval=time_evaluations)
    
    omega = solution.y[0]
    phi = solution.y[1]
    vx = solution.y[2]
    vy = solution.y[3]

    heading = radiant_to_degrees(phi)
    v_vessel = np.sqrt(vx**2 + vy**2 ) 
    V_X = vx * np.cos(phi) - vy * np.sin(phi)
    V_Y = vy * np.cos(phi) + vx * np.sin(phi)

    # Coordinates in the intertial system
    X = cumulative_trapezoid(V_X, solution.t, initial = 0) + start_point[-2]
    Y = cumulative_trapezoid(V_Y, solution.t, initial= 0) + start_point[-1]

    end_point = [omega[-1], phi[-1], vx[-1], vy[-1], X[-1], Y[-1]]
    
    return end_point, X, Y, heading, v_vessel

