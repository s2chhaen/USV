/*
 * main.h: Alle Inklusionen der Bibliothek und Konfigurationen für main.c
 *
 * Created: 15.06.2023 13:07:06
 * Author: Thach
 * Version: 1.0
 * Revision: 1.0
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "./ATMegaXX09/ATMegaXX09Clock.h"
#include "./ATMegaXX09/USART/USART.h"
#include "slaveDevice.h"
#include "timerUnit.h"

#define DISBL_MPC_MODE false
#define DISBL_SYNC_TX false

#endif /* MAIN_H_ */