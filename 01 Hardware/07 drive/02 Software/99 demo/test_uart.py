# -*- coding: utf-8 -*-
"""
Realisierung von 8 möglichen UART-Schnittstellen mittels PIO
Bei der Umsetzung muss darauf geachtet werden, dass für jede UART-Realisierung
eine eigene Statemachine und ein eigener DMA-Kanal verwendet wird.

Program history
09.10.2024    V. 00.01    Start

@author: Prof. Grabow (grabow@amesys.de)
"""
__version__ = '00.01'
__author__ = 'Joe Grabow'

import time
from pio import uart_dma_rx as uart_rx
from pio import uart_dma_tx as uart_tx
import uarray as arr

    
def callback(buffer, recv):
    print(f"received: {recv} Bytes in {buffer}")


# Initialisierung der TX UART
BAUD         = 115200
tx_pin       = 16  # verwendeter Hardwarepin des RPi Pico
statemachine = 0  # [0 ...  7] Tx und Rx müssen sich unterscheiden
dmaChannel   = 0  # [0 ... 11] Tx und Rx müssen sich unterscheiden
data_size    = 11  # Anzahl der Datenbytes welche gesendet werden

data = arr.array("B", [0] * data_size)  # Datenarray zum Senden
for i in range(data_size):  # füllt Array mit ASCI-Zeichen
    data[i] = 65 + i  # ab Buchstabe A

# Initialisierung der Statemanine
tx_1 = uart_tx.UART_PIO_DMA_TX(statemachine = statemachine,
                       dmaChannel = dmaChannel,
                       tx_pin = tx_pin,
                       baud=BAUD)


# Initialisierung der RX UART
BAUD         = 115200 
rx_pin       = 17  # verwendeter Hardwarepin des RPi Pico
buffer_size  = 20  # Empfangspuffer
statemachine = 1  # [0 ...  7] Tx und Rx müssen sich unterscheiden
dmaChannel   = 1  # [0 ... 11] Tx und Rx müssen sich unterscheiden

a = arr.array("b", [0] * 5)
rx_1 = uart_rx.UART_PIO_DMA_RX(statemachine=statemachine,            
                dmaChannel=dmaChannel,
                rx_pin=rx_pin,
                buffer_size = buffer_size,
                # callback = callback,
                baud=BAUD)


# TX Routine zum Senden eines Datenblocks
def send(data):
    print(f"sending {tx_1.write(data, data_size)} Bytes")
    while tx_1.dma_busy():
        print(".", end = "")
        time.sleep(.001)
    time.sleep(.001)
      

print("\nWaiting for UART-Data ... Ctrl-C to stop.")
rx_1.active(1)  # UART RX aktivieren 

send(data)  # Datenblock senden
print("TX done\n")


# Endlosschleife um Daten über UART RX zu empfangen und auszugeben
try:    
    while True:
        if rx_1.ready():
            d, cnt = rx_1.get_buffer()
            for i in range(cnt):
                #print(f"{d[i]:02X}", end = " ")  # Rx Zeichen als Hex
                print(f"{chr(d[i])}", end=" ")  # Rx Zeichen als ASCII
                print(f" -> #{cnt}")  # Anzahl der empfangenen Zeichen
        rx_1.check()
        
except KeyboardInterrupt:
    pass

rx_1.active(0)
print("\ndone")
