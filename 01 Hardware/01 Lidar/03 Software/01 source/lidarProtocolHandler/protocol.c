#include "protocol.h"
static uint16_t buffer[MAX_DATA_LIDAR] = {0};
static inline void dataExtract(uint16_t data, uint16_t idx){
    buffer[idx%MAX_DATA_LIDAR] = data;
}

void getExtractedData(int32_t* data, uint16_t* dataLen){
    if((*dataLen)>=MAX_DATA_LIDAR){
        *dataLen = MAX_DATA_LIDAR;
        //memcpy((uint16_t*)data,buffer,MAX_DATA_LIDAR*sizeof(uint16_t));
        for(int i = 0; i< MAX_DATA_LIDAR; i++){
            data[i] = (int32_t)buffer[i];
            //printf("Bereich = %.2f Grad \n",0.50*i);
            //printf("Wert[%.2lf] = %d cm \n",0.50*i,data[i]);
        }
    }
}

 //Kommando-10h-Req - OK
void hwdInitAndResetReq(uint8_t* protocol, uint16_t* len){
    const uint16_t kernelLen = 0x01;
    uint16_t crc16Part = 0;
    if((*len) >= 7){
        *len = 7;
        protocol[REQ_START_BYTE] = ASCII_STX_CHAR;
        protocol[REQ_ADDR_BYTE] = ADDR_DEFAULT;
        protocol[REQ_LENGTH_L_BYTE] = kernelLen & 0xff;
        protocol[REQ_LENGTH_H_BYTE] = kernelLen >> 8;
        protocol[REQ_CMD_BYTE] = 0x10;
        crc16Part = crc16(protocol,5,DEFAULT_CRC16_POLYNOM);
        protocol[REQ_CMD_BYTE+1] = crc16Part & 0xff;//CRC16-Low
        protocol[REQ_CMD_BYTE+2] = crc16Part >> 8;//CRC16-High
    }
}

 //Komando-10h-Rsp - OK
void hwdInitAndResetRsp(uint8_t* data, uint16_t dataLen){
}

//Kommando 0x30-0x01 und 0x36
uint8_t checkData(uint8_t* data, uint16_t dataLen){
    uint16_t idx = 0;

    uint16_t data2buffer = 0;
    printf("Daten vom Lidar ohne Filterung \n");
    printf("------------------------------------------------------------------\n");
    printf("Byte %d = 0x%02X\n",idx,data[idx]);
    uint8_t checkACK = data[idx] == ASCII_ACK_CHAR; //OK
    printf("ACK = %s\n", checkACK?"true":"false");
    printf("-----\n");
    idx++;
    //Start
    uint8_t checkSTX = data[idx] == ASCII_STX_CHAR; //OK
    printf("Byte %d = 0x%02X\n",idx,data[idx]);
    printf("Start bit = %s\n", checkSTX?"true":"false");
    printf("-----\n");
    idx++;
    //Addr
    printf("Byte %d = 0x%02X\n",idx,data[idx]);
    uint8_t checkAddr = data[idx] == (ADDR_DEFAULT+0x80); //OK
    printf("Addr = %s\n", checkAddr?"true":"false");
    printf("-----\n");
    idx++;
    //Protocol Length
    printf("Byte %d and %d = 0x%02X 0x%02X\n",idx,idx+1,data[idx],data[idx+1]);
    uint16_t cmdAndDataLen = (data[idx]<<0)|(data[idx+1]<<8);
    printf("Length of Command + Data = %d\n", cmdAndDataLen);
    printf("-----\n");
    idx += 2;
    //command
    printf("Byte %d = 0x%02X\n",idx,data[idx]);
    uint8_t cmd = data[idx] - 0x80;
    printf("Command = 0x%02X\n", cmd);
    printf("-----\n");
    idx++;
    //data-informations
    //Block A
#define PROTOCOL_C_SEG_1
#ifdef PROTOCOL_C_SEG_1
    printf("Byte %d and %d = 0x%02X 0x%02X\n",idx,idx+1,data[idx],data[idx+1]);
    printf("Datenkonfiguration: \n\n");
    uint16_t blockA = (data[idx])|(data[idx+1]<<8);
    uint8_t dataUnit = (uint8_t)((blockA & (UNIT_BIT_BM))>>14);
    printf("Einheit = %s \n",(!dataUnit)?"cm":"mm");
    uint8_t scanType = (uint8_t)((blockA & (SCAN_TYPE_BM))>>13);
    printf("Scan-Typ = %s \n",(!scanType)?"komplett":"Teilscan");
    uint8_t scanRes = (uint8_t)((blockA & (PARTIAL_SCAN_RES_BM))>>11);
    uint8_t str[10] ={0};
    if(scanRes==0b00){
        memcpy(str,"x.00Grad",sizeof("x.00Grad")/sizeof(uint8_t));
    } else if(scanRes==0b01){
        memcpy(str,"x.25Grad",sizeof("x.25Grad")/sizeof(uint8_t));
    } else if(scanRes==0b10){
        memcpy(str,"x.50Grad",sizeof("x.50Grad")/sizeof(uint8_t));
    } else if(scanRes==0b11){
        memcpy(str,"x.75Grad",sizeof("x.75Grad")/sizeof(uint8_t));
    } else{
        memcpy(str,"Error",sizeof("x.75Grad")/sizeof(uint8_t));
    }
    printf("Messwerte gehoert zum Teilscan = %s \n",str);
    uint16_t mValLen = blockA & VAL_NO_BM;
    printf("Laenge von Daten = %d \n",mValLen);
    printf("-----\n");
    idx += 2;
#endif // PROTOCOL_C_SEG_1

#ifdef PROTOCOL_C_SEG_2
    uint16_t mValLen = 722/2;
    printf("Anzahl der Mittelungen = %d\n",data[idx]);
    idx++;
    uint16_t config = data[idx] & (data[idx+1]<<8);
    printf("Einheit = %s\n",(config & (0x03<<14))?"mm":"cm");
    idx += 2;
#endif // PROTOCOL_C_SEG_1
    //Data
    printf("Byte %d\n",idx);
    printf("Datenbereich: \n\n");
    //const float incrStep = 0.50f;
    for(int i = 0; i < mValLen; i++){
        //printf("Byte %d and %d = 0x%02X 0x%02X\n",idx,idx+1,data[idx],data[idx+1]);
        data2buffer = (data[idx+1]<<8) | (data[idx]);
        data2buffer &= 0x1FFF;
        dataExtract(data2buffer,i);
        //printf("Wert[%.2lf] = %d cm \n",0.50f*i,data2buffer);
        idx+=2;
    }
    printf("-----\n");

    printf("Byte %d = 0x%02X\n",idx,data[idx]);
    printf("LMS Status = 0x%02X \n",data[idx]);
    printf("-----\n");
    idx++;
    //checksum-16-Bits
    printf("Byte %d and %d = 0x%02X 0x%02X\n",idx,idx+1,data[idx],data[idx+1]);
    uint16_t crcCal = crc16(&(data[1]),dataLen-3,DEFAULT_CRC16_POLYNOM);
    uint16_t crcProtocol = (data[idx]) | (data[idx+1]<<8);
    printf("checksum = %s\n", (crcCal == crcProtocol)?"true":"false");
    printf("-----\n");
    idx+=2;
    printf("dataLen = %d \n",idx);
    printf("------------------------------------------------------------------\n");
    return checkACK && checkSTX && checkAddr && (crcCal == crcProtocol);
}


//Vom Beginn zu Ende
//Kommando 02 00 01 00 31 15 12 => 31h
void checkConfig(uint8_t* data, uint16_t dataLen){
    uint16_t temp = 0;
    uint16_t i = 0;
    uint8_t sum = 0;
    printf("Konfiguration vom Lidar : von Begin zu Ende gelesen\n");
    printf("------------------------------------------------------------------\n");
    printf("Byte = %d\n",i);
    uint8_t checkACK = data[i] == ASCII_ACK_CHAR;
    i++;
    printf("ACK = %s\n", checkACK?"true":"false");
    printf("-----\n");
    //Start
    printf("Byte = %d\n",i);
    uint8_t checkSTX = data[i] == ASCII_STX_CHAR;
    i++;
    printf("Start bit = %s\n", checkSTX?"true":"false");
    printf("-----\n");
    //Addr
    printf("Byte = %d\n",i);
    uint8_t checkAddr = data[i] == (ADDR_DEFAULT+0x80);
    i++;
    printf("Addr = %s\n", checkAddr?"true":"false");
    printf("-----\n");
    //Protocol Length
    printf("Byte = %d\n",i);
    uint16_t cmdAndDataLen = (data[i]<<0)|(data[i+1]<<8);
    i+=2;
    printf("value = 0x%02X 0x%02X \n",data[i],data[i+1]);
    printf("Length of Command + Data = %d\n", cmdAndDataLen);
    printf("-----\n");
    //command
    printf("Byte = %d\n",i);
    uint8_t cmd = data[i] - 0x80;
    i++;
    printf("Command = 0x%02X\n", cmd);
    printf("-----\n");
    //data-informations

    //Version Block A
    printf("Byte = %d\n",i);
    uint8_t* str = NULL;
    str = (uint8_t*)malloc((CONFIG_BLK_A+1)*sizeof(uint8_t));
    str[CONFIG_BLK_A] = '\0';
    memcpy(str,&data[i],CONFIG_BLK_A);
    printf("Software Version Block A = %s \n",str);
    i += CONFIG_BLK_A;
    free(str);
    printf("-----\n");

    //OP-Mode Block B
    printf("Byte = %d\n",i);
    temp = data[i];
    printf("Operating Mode = 0x%02X \n",(uint8_t)temp);
    i += CONFIG_BLK_B;
    printf("-----\n");

    //Status block C
    printf("Byte = %d\n",i);
    temp = data[i];
    printf("Status = %s \n",(temp)?"defected":"normal");
    i += CONFIG_BLK_C;
    printf("-----\n");

    //Block D
    printf("Byte = %d\n",i);
    printf("block D = ");
    sum = CONFIG_BLK_D;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block E
    printf("Byte = %d\n",i);
    i += CONFIG_BLK_E;
    printf("BlockE = %s\n",data[i]?"Standard":"Sonder");
    printf("-----\n");
    //Block F
    printf("Byte = %d\n",i);
    printf("block F = ");
    sum = CONFIG_BLK_F;
    for(int j = 0; j < sum; j++){
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block G
    printf("Byte = %d\n",i);
    printf("block G = ");
    sum = CONFIG_BLK_G;
    for(int j = 0; j < sum; j++){
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //BLock H
    printf("Byte = %d\n",i);
    printf("block H = ");
    sum = CONFIG_BLK_H;
    for(int j = 0; j < sum; j++){
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block I
    printf("Byte = %d\n",i);
    printf("block H = ");
    sum = CONFIG_BLK_I;
    for(int j = 0; j < sum; j++){
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block J
    printf("Byte = %d\n",i);
    temp = 0;
    temp = data[i]| (data[i+1]<<8);
    printf("BlockJ = 0x%04X",(uint16_t)temp);
    i += CONFIG_BLK_J;
    printf(" => Drehzahl = %" PRIu64 " U/min\n", 1000000UL*60/(((uint64_t)temp)*90));
    printf("-----\n");

    //Block K: Unterschied bei Telegram-Datenblatt mit verschiedene Version
    printf("Byte = %d\n",i);
    printf("Block K = ");
    sum = CONFIG_BLK_K + OFFSET_BYTES;
    for(int j = 0; j < sum; j++){
        if((j%9)==0){
            printf("\n");
        }
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block L bis T
    printf("Byte = %d\n",i);
    printf("Block L bis T = ");
    sum = CONFIG_BLK_L + CONFIG_BLK_M + CONFIG_BLK_N \
        + CONFIG_BLK_O + CONFIG_BLK_P + CONFIG_BLK_Q \
        + CONFIG_BLK_R + CONFIG_BLK_S + CONFIG_BLK_T;
    for(int j = 0; j < sum; j++){
        if((j%9)==0){
            printf("\n");
        }
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block U
    printf("Byte = %d\n",i);
    temp = data[i] | (data[i+1]<<8);
    printf("BlockU = 0x%04X",(uint16_t)temp);
    i += CONFIG_BLK_U;
    printf(" => aktueller Winkel = %d Grad\n", (uint32_t)temp);
    printf("-----\n");

    //Block V bis A1
    printf("Byte = %d\n",i);
    printf("Block V bis A1 =");
    sum = CONFIG_BLK_V + CONFIG_BLK_W \
        + CONFIG_BLK_X + CONFIG_BLK_Y \
        + CONFIG_BLK_Z + CONFIG_BLK_A1;
    for(int j = 0; j < sum; j++){
        if((j%5)==0){
            printf("\n");
        }
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block A2
    printf("Byte = %d\n",i);
    printf("BlockA2 = 0x%02X",data[i]);
    printf(" => Messmodus = 0x%02X\n",data[i]);
    i += CONFIG_BLK_A2;
    temp = 0;
    printf("-----\n");

    //Block A3 bis A4
    printf("Byte = %d\n",i);
    printf("Block A3 bis A4 =");
    sum = CONFIG_BLK_A3 + CONFIG_BLK_A4;
    for(int j = 0; j < sum; j++){
        if((j%5)==0){
            printf("\n");
        }
        printf("0x%02X ",data[i+j]);
    }
    i += sum;
    printf("\n");
    printf("-----\n");

    //Block A5
    printf("Byte = %d\n",i);
    printf("BlockA5 = ");
    temp |= data[i]|(data[i+1]<<8);
    printf("0x%04X ",(uint32_t)temp);
    i += CONFIG_BLK_A5;
    printf(" => Scanwinkel = %d Grad\n", (uint32_t)temp);
    temp = 0;
    printf("-----\n");
    //Block A6
    printf("Byte = %d\n",i);
    printf("BlockA6 = ");
    temp |= data[i]|(data[i+1]<<8);
    printf("0x%04X ",(uint32_t)temp);
    i += CONFIG_BLK_A6;
    printf(" => Winkelaufloesung = %d in 1/100 Grad\n", (uint32_t)temp);
    temp = 0;
    printf("-----\n");
    //Block A7 bis A9 und B1
    printf("Byte = %d\n",i);
    printf("BlockA7 bis A9 und B1 \n");
    i += CONFIG_BLK_A7 + CONFIG_BLK_A8 + CONFIG_BLK_A9 + CONFIG_BLK_B1;
    printf("-----\n");

    //Block B2
    printf("Byte = %d\n",i);
    temp = data[i]|(data[i+1]<<8);
    printf("BlockB2 = 0x%04X \n",(uint32_t)temp);
    i += CONFIG_BLK_B2;
    printf("-----\n");

    //Block B3
    printf("Byte = %d\n",i);
    printf("BlockB3 = 0x%02X \n",data[i]);
    i += CONFIG_BLK_B3;
    printf("-----\n");

    //Block B4
    printf("Byte = %d\n",i);
    printf("BlockB4 = 0x%02X \n",data[i]);
    i += CONFIG_BLK_B4;
    printf("-----\n");

    //Block B5
    printf("Byte = %d\n",i);
    printf("BlockB5 = 0x%02X \n",data[i]);
    i += CONFIG_BLK_B5;
    printf("-----\n");

    //Block B6
    printf("Byte = %d\n",i);
    printf("BlockB6 = 0x%02X \n",data[i]);
    i += CONFIG_BLK_B6;
    printf("-----\n");

    //Block B7
    printf("Byte = %d\n",i);
    printf("BlockB7 = 0x%02X \n",data[i]);
    i += CONFIG_BLK_B7;
    printf("-----\n");

    //Block B8
    printf("Byte = %d\n",i);
    printf("BlockB8 = 0x%02X \n",data[i]);
    i += CONFIG_BLK_B8;
    printf("-----\n");

    //Version Block B9
    printf("Byte = %d\n",i);
    str = NULL;
    str = (uint8_t*)malloc((CONFIG_BLK_B9+1)*sizeof(uint8_t));
    str[CONFIG_BLK_B9] = '\0';
    memcpy(str,&data[i],CONFIG_BLK_B9);
    printf("EEPROM Version Block B9 = %s \n",str);
    i += CONFIG_BLK_B9;
    free(str);
    printf("-----\n");

    //Block C1
    printf("Byte = %d\n",i);
    printf("block C1 = ");
    sum = CONFIG_BLK_C1;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block C2
    printf("Byte = %d\n",i);
    printf("block C2 = ");
    sum = CONFIG_BLK_C2;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block C3
    printf("Byte = %d\n",i);
    printf("block C3 = ");
    sum = CONFIG_BLK_C3;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block C4
    printf("Byte = %d\n",i);
    printf("block C4 = ");
    sum = CONFIG_BLK_C4;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block C5
    printf("Byte = %d\n",i);
    printf("block C5 = ");
    sum = CONFIG_BLK_C5;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block C6
    printf("Byte = %d\n",i);
    printf("block C6 = ");
    sum = CONFIG_BLK_C6;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Block C7
    printf("Byte = %d\n",i);
    printf("block C7 = ");
    sum = CONFIG_BLK_C7;
    for(int j = 0; j<sum;j++){
        printf("0x%02X ",data[i+j]);
    }
    printf("\n");
    i += sum;
    printf("-----\n");

    //Status LMS
    printf("Byte = %d\n",i);
    uint8_t lmsStatus = data[i];
    i += CONFIG_STATUS_LMS;
    printf("LMS-Status = 0x%02X \n", lmsStatus);
    printf("-----\n");

    //Checksum
    uint16_t crcCal = crc16(&(data[1]),dataLen-3,DEFAULT_CRC16_POLYNOM);
    uint16_t crcProtocol = (data[i]) | (data[i+1]<<8);
    printf("checksum = %s\n", (crcCal == crcProtocol)?"true":"false");
    i += 2;
    printf("------------------------------------------------------------------\n");
    printf("length inclusive ACK = %d \n",i);
}

