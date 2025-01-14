/*
 * main.h
 * Beschreibung: Header-Datei der main.c-Datei
 * Created: 10/26/2023 4:34:31 PM
 * Author: Thach
 * Version: 1.1
 * Revision: 1.2
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "importedFiles/wr_qFormFilterCof.h"
#include "Math/checksum.h"
#include "ATMegaXX09/ATMegaXX09Clock.h"
#include "ATMegaXX09/USART/USART.h"
#include "timerUnit.h"
#include "lidarHandler.h"
#include "lidarDataPrc.h"
#include "usvDataHandler.h"

#define LIDAR_CHECKSUM_POLYNOM 0x8005
#define USV_CHECKSUM_POLYNOM 0xD5
#define FIL_OUTPUT_DATA_FP_LEN_BIT 4

#endif /* MAIN_H_ */