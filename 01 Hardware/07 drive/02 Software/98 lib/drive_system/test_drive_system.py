# -*- coding: utf-8 -*-
"""
Created on Thu Oct 17 10:10:13 2024

Program history
17.10.2024    V. 00.01    Start

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.01'
__author__ = 'Joe Grabow'


# test_drive_system.py

from drive_system import DriveSystem

def test_drive_A():
    ds = DriveSystem(1, 0.5, 0.1)
    assert ds.drive_A() == (-2 * 1 + 1) * 0.5  # Expected: -0.5

    ds = DriveSystem(0, 0.05, 0.1)
    assert ds.drive_A() == 0  # Expected: 0 (wegen Trust nahe 0 und Rudder = 0)

    ds = DriveSystem(-1, 0.5, 0.1)
    assert ds.drive_A() == 0.5  # Expected: Trust zurückgeben, weil Rudder <= 0

def test_drive_B():
    ds = DriveSystem(-1, 0.5, 0.1)
    assert ds.drive_B() == (2 * -1 + 1) * 0.5  # Expected: 0

    ds = DriveSystem(0, 0.05, 0.1)
    assert ds.drive_B() == 0  # Expected: Rudder zurückgeben, weil Trust nahe 0

    ds = DriveSystem(1, 0.5, 0.1)
    assert ds.drive_B() == 0.5  # Expected: Trust zurückgeben, weil Rudder >= 0

if __name__ == "__main__":
    test_drive_A()
    test_drive_B()
    print("Alle Tests bestanden!")

