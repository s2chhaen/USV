"""
Created on Wed Oct 16 16:47:39 2024

Program history
17.10.2024    V. 00.01    Start

@author: Prof. Grabow (grabow@amesys.de)
"""

__version__ = '00.01'
__author__ = 'Joe Grabow'

# drive_system.py

class DriveSystem:
    def __init__(self, rudder, trust, hysterese):
        self.rudder = rudder
        self.trust = trust
        self.hysterese = hysterese

    def drive_A(self):
        if abs(self.trust) <= self.hysterese:  # Trust ist nahe 0
            return -self.rudder
        elif self.rudder > 0:
            return (-2 * self.rudder + 1) * self.trust
        else:
            return self.trust

    def drive_B(self):
        if abs(self.trust) <= self.hysterese:  # Trust ist nahe 0
            return self.rudder
        elif self.rudder < 0:
            return (2 * self.rudder + 1) * self.trust
        else:
            return self.trust
