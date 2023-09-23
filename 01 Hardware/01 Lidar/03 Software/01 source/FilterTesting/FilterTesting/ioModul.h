#ifndef _IOMODUL_H_
#define _IOMODUL_H_

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stddef.h>

#define PTR_LEN_BYTES 9
#define BUFFER_MAX_LENGTH (1<<PTR_LEN_BYTES)
#define MAX_LEN_STRING 15


//Zeichenfolge-Array (Max. Länge der Zeichenfolge = 15)
typedef struct ringBuffer4Str{
    uint8_t data[BUFFER_MAX_LENGTH][MAX_LEN_STRING];
    uint8_t dataLen[BUFFER_MAX_LENGTH];
    uint16_t readIdxPtr:PTR_LEN_BYTES;
    uint16_t writeIdxPtr:PTR_LEN_BYTES;
    uint8_t fullFlag:1;
}ringBuffer4Str_t;

extern void readFile();
extern void getData(int32_t* data, uint16_t* dataLen);

#endif // _IOMODUL_H_
