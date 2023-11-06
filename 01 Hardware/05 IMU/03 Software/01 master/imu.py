# coding: utf-8
"""
Program history
06.11.2023    V. 1.0    Start

IMU measuring system for USV project
https://github.com/Joe-Grabow/USV

@author: Joe Grabow (grabow@amesys.de)
"""
__version__ = '1.0'
__author__ = 'Joe Grabow'

from machine import Timer, Pin
from picozero import Button
from time import sleep

led = Pin(2, Pin.OUT, value=0)  # GPIO2 for LED
button = Button(3)  # GPIO3 for Calibration Button

def flash(state):
    if state == 0:  # LED off
        blink.deinit()  # flash off
        led.off()
    elif state == 1:  # flash slow
        blink.init(period=500, mode=Timer.PERIODIC, callback=lambda t:led.value(not led.value()))
    elif state == 2:  # flash fast
        blink.init(period=200, mode=Timer.PERIODIC, callback=lambda t:led.value(not led.value()))
    elif state == 3:  # LED on
        blink.deinit()  # flash off
        led.on()  
    return()

def wait_for_press():
    while not button.is_pressed:
        pass

# sub-programme for calibration
def calibration():
        print("state 1: Calibration Step 1")
        flash(1)
        sleep(1)
        wait_for_press()
        
        print("state 2: Calibration Step 2")
        flash(2)
        sleep(1)
        wait_for_press()
        
        print("state 3: Calibration Step 3")
        flash(3)

# sub-programme for measurement
def measurement():
        print("Measurement")

blink = Timer(period=500, mode=Timer.PERIODIC, callback=lambda t:led.value(not led.value()))
flash(0)

while True:
    sleep(0.2)
    if button.is_pressed:
        calibration()
    else:
        measurement()