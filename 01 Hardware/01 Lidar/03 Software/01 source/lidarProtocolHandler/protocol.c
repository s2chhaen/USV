#include "protocol.h"
static uint16_t buffer[MAX_DATA_LIDAR] = {0};
//Kommando 0x30-0x01 und 0x36
uint8_t checkData(uint8_t* data, uint16_t dataLen){
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

