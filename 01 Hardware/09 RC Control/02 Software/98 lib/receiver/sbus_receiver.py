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

from machine import UART, Pin
import array


class SBUSReceiver:
    def __init__(self, uart_port):
        # Initialisierung des UART für SBUS (invertiertes RX-Signal, wie für SBUS erforderlich)
        self.sbus = UART(uart_port, baudrate=100000, tx=Pin(8), rx=Pin(9), bits=8, parity=0, stop=2, invert=UART.INV_RX)

        # Konstanten
        self.START_BYTE = 0x0F  # Startbyte 0x0F
        self.END_BYTE = 0x00    # Endbyte 0x00
        self.SBUS_FRAME_LEN = 25
        self.SBUS_NUM_CHANNELS = 18
        self.OUT_OF_SYNC_THD = 10  # Schwellenwert für erneute Synchronisation
        self.SBUS_SIGNAL_OK = 0
        self.SBUS_SIGNAL_LOST = 1
        self.SBUS_SIGNAL_FAILSAFE = 2

        # Initialisierung der Variablen
        self.validSbusFrame = 0
        self.lostSbusFrame = 0
        self.frameIndex = 0
        self.resyncEvent = 0
        self.outOfSyncCounter = 0
        self.sbusBuff = bytearray(1)  # Puffer für ein Byte (wird zum Synchronisieren verwendet)
        self.sbusFrame = bytearray(self.SBUS_FRAME_LEN)  # Puffer für ein vollständiges SBUS-Frame
        self.sbusChannels = array.array('H', [0] * self.SBUS_NUM_CHANNELS)  # RC-Kanäle
        self.isSync = False
        self.startByteFound = False
        self.failSafeStatus = self.SBUS_SIGNAL_FAILSAFE

    def get_rx_channels(self):
        """Gibt die aktuellen SBUS-Kanalwerte zurück."""
        return self.sbusChannels

    def get_rx_channel(self, num_ch):
        """Gibt den Wert eines bestimmten SBUS-Kanals zurück."""
        if 0 <= num_ch < self.SBUS_NUM_CHANNELS:
            return self.sbusChannels[num_ch]
        return None

    def get_failsafe_status(self):
        """Gibt den aktuellen Failsafe-Status zurück."""
        return self.failSafeStatus

    def get_rx_report(self):
        """Gibt eine Zusammenfassung der Frames-Dekodierungsstatistiken zurück."""
        return {
            'Valid Frames': self.validSbusFrame,
            'Lost Frames': self.lostSbusFrame,
            'Resync Events': self.resyncEvent
        }

    def decode_frame(self):
        """Dekodiert ein SBUS-Frame in die entsprechenden Kanalwerte."""
        # Kanäle zurücksetzen
        for i in range(self.SBUS_NUM_CHANNELS - 2):
            self.sbusChannels[i] = 0

        byte_in_sbus = 1  # Start nach dem Startbyte
        bit_in_sbus = 0
        ch = 0
        bit_in_channel = 0

        for i in range(0, 175):  # Dekodiere die Kanäle (11 Bit pro Kanal)
            if self.sbusFrame[byte_in_sbus] & (1 << bit_in_sbus):
                self.sbusChannels[ch] |= (1 << bit_in_channel)

            bit_in_sbus += 1
            bit_in_channel += 1

            if bit_in_sbus == 8:
                bit_in_sbus = 0
                byte_in_sbus += 1

            if bit_in_channel == 11:
                bit_in_channel = 0
                ch += 1

        # Dekodiere die digitalen Kanäle
        self.sbusChannels[self.SBUS_NUM_CHANNELS - 2] = 1 if self.sbusFrame[self.SBUS_FRAME_LEN - 2] & (1 << 0) else 0
        self.sbusChannels[self.SBUS_NUM_CHANNELS - 1] = 1 if self.sbusFrame[self.SBUS_FRAME_LEN - 2] & (1 << 1) else 0

        # Failsafe-Status dekodieren
        self.failSafeStatus = self.SBUS_SIGNAL_OK
        if self.sbusFrame[self.SBUS_FRAME_LEN - 2] & (1 << 2):
            self.failSafeStatus = self.SBUS_SIGNAL_LOST
        if self.sbusFrame[self.SBUS_FRAME_LEN - 2] & (1 << 3):
            self.failSafeStatus = self.SBUS_SIGNAL_FAILSAFE

    def get_sync(self):
        """Versucht, die Synchronisation mit dem SBUS-Frame zu erreichen."""
        if self.sbus.any() > 0:  # Daten im Buffer
            if self.startByteFound:
                if self.frameIndex == (self.SBUS_FRAME_LEN - 1):
                    # Lese das Endbyte
                    self.sbus.readinto(self.sbusBuff, 1)
                    if self.sbusBuff[0] == self.END_BYTE:
                        # Erfolgreich synchronisiert
                        self.startByteFound = False
                        self.isSync = True
                        self.frameIndex = 0
                    else:
                        # Keine Synchronisation erreicht
                        self.startByteFound = False
                else:
                    # Lese Byte für Byte weiter, bis das Frame-Ende erreicht ist
                    self.sbus.readinto(self.sbusBuff, 1)
                    self.frameIndex += 1
            else:
                # Suche nach dem Startbyte
                self.frameIndex = 0
                self.sbus.readinto(self.sbusBuff, 1)
                if self.sbusBuff[0] == self.START_BYTE:  # Startbyte erkannt
                    self.startByteFound = True  
                    self.frameIndex += 1

    def get_new_data(self):
        """ Ruft neue SBUS-Daten ab und dekodiert sie."""
        if self.isSync:
            if self.sbus.any() >= self.SBUS_FRAME_LEN:
                # Lese ein komplettes SBUS-Frame
                self.sbus.readinto(self.sbusFrame)
                if self.sbusFrame[0] == self.START_BYTE and self.sbusFrame[self.SBUS_FRAME_LEN - 1] == self.END_BYTE:
                    # Gültiges SBUS-Frame
                    self.validSbusFrame += 1
                    self.outOfSyncCounter = 0
                    self.decode_frame()
                else:
                    # Ungültiges Frame
                    self.lostSbusFrame += 1
                    self.outOfSyncCounter += 1

                # Überprüfen, ob erneute Synchronisation notwendig ist
                if self.outOfSyncCounter > self.OUT_OF_SYNC_THD:
                    self.isSync = False
                    self.resyncEvent += 1
            else:
                # Nicht genügend Daten vorhanden, versuche Synchronisation wiederherzustellen
                self.get_sync()
        else:
            # Versuche, die Synchronisation wiederherzustellen
            self.get_sync()
