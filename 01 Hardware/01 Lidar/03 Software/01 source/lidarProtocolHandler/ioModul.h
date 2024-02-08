/**
 * ioModul.h
 * Description: Header-Datei für Datenlesen aus der Text-Datei und
 * -schreiben in Form der Text-Datei
 * Author: Thach
 * Created: 10/05/2023
 * Version: 1.0
 * Revision: 1.0
 */

#ifndef _IOMODUL_H_
#define _IOMODUL_H_

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stddef.h>
#include <string.h>


#define PTR_LEN_BYTES 10
#define BUFFER_MAX_LENGTH (1<<PTR_LEN_BYTES)

enum fileFormat{
    TEXT_FORM,
    CSV_FORM
};

extern void readFile(char* path);
extern void getData(uint8_t* data, uint16_t* dataLen);
extern void writeFileInt32(uint8_t* name, uint16_t len, uint8_t format, int32_t* value, uint16_t valLen);
extern void writeFileInt64(uint8_t* name, uint16_t len, uint8_t format, int64_t* value, uint16_t valLen);

#endif // _IOMODUL_H_
