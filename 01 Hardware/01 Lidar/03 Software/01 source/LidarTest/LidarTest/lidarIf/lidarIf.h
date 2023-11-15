/*
 * lidarIf.h
 *
 * Created: 10/19/2023 2:58:05 PM
 *  Author: Thach
 */ 


#ifndef LIDARIF_H_
#define LIDARIF_H_

#include <stdint.h>
#include <string.h>
#include "asciiTable.h"
#include "../Math/checksum.h"

#define LMS2XX 6
#define DEFAULT_VERSION ((1<<8)|(10<<0));

typedef uint8_t (*lidarIf_rxFunc_p)(uint8_t* data, uint16_t length);
typedef uint8_t (*lidarIf_txFunc_p)(uint8_t* data, uint16_t length);

#endif /* LIDARIF_H_ */