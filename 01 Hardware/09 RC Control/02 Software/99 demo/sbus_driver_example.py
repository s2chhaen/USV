# -*- coding: utf-8 -*-
"""
Created on Wed Oct 16 13:37:32 2024

Program history
16.10.2023    V. 00.01    Start

@author: Grabow

The idea of this realization is from [1]. However, it has been completely 
revised and adapted to the RPi Pico. 
This concerns the inversion of the RX pin in the SBUS protocol and 
the error security. 

[1] https://github.com/Sokrates80/sbus_driver_micropython

"""
__version__ = '00.01'
__author__ = 'Joe Grabow'


import machine
import utime
from receiver.sbus_receiver import SBUSReceiver


# Globale Variablen zum Verwalten von Flags
updateLed = False
update_rx = False

# Callback-Funktion für das Empfangen neuer SBUS-Daten
def update_rx_data(timer):
    global update_rx
    update_rx = True

# Callback-Funktion für den Status der LED
def status_led(timer):
    global updateLed
    updateLed = True
    led.toggle()


# Init the SBUS driver on UART port 1
sbus = SBUSReceiver(1)

# Initialisiere die LED (z.B. auf GPIO 16)
led = machine.Pin(16, machine.Pin.OUT)

# Timer für SBUS-Datenverarbeitung (3ms )
timRx = machine.Timer()
timRx.init(period=3, mode=machine.Timer.PERIODIC, callback=update_rx_data)

# Timer für die Status-LED (100 Millisekunden)
tim1 = machine.Timer()
tim1.init(period=100, mode=machine.Timer.PERIODIC, callback=status_led)

# Hauptschleife
while True:
    if update_rx:
        sbus_data = sbus.get_new_data()
        #if sbus_data:
            #print("Neue SBUS-Daten empfangen:", sbus_data)
        update_rx = False  # Flag zurücksetzen

    if updateLed:
        # SBUS-Informationen alle 100 Millisekunden ausgeben
        #print("SBUS-Kanäle:", sbus.get_rx_channel(0))
        print("FailSave:", sbus.get_failsafe_status())  # Failsave 2 = außerhalb Reichweite
        report = sbus.get_rx_report()
        #print(report)
        print("SBUS-Kanäle:", sbus.get_rx_channels())
        updateLed = False  # Flag zurücksetzen

    utime.sleep_ms(1)  # Kleine Pause, um CPU zu entlasten
