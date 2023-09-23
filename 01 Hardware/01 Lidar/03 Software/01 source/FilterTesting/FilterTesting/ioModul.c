#include "ioModul.h"

static ringBuffer4Str_t buffer = {{{0}},{0},0,0,0};

static uint16_t getAvaDataLen(){
    uint16_t result = 0;
    int32_t temp = 0;
    if(buffer.fullFlag){
        result = 512;
    } else{
        temp = (buffer.writeIdxPtr - buffer.readIdxPtr)%BUFFER_MAX_LENGTH;
        temp = (temp>0)?temp:(temp+BUFFER_MAX_LENGTH);
        result = (uint16_t) temp;
    }
    return result;
}

static inline int32_t min(int32_t a, int32_t b){
    return (a<b)?a:b;
}

void readFile(){
    FILE *input = fopen("input_fixed.txt","r");
    if(input!=NULL){
        uint8_t stringPtr = 0;
        int32_t character = 0;
        while((character = fgetc(input)) != EOF){
            if(character=='\n'){
                if(buffer.fullFlag){
                    printf("Buffer voll \n");
                } else{
                    buffer.dataLen[buffer.writeIdxPtr++] = stringPtr;
                    stringPtr = 0;
                    buffer.fullFlag = (buffer.writeIdxPtr==buffer.readIdxPtr)?1:0;
                }
            }else{
                if(stringPtr<MAX_LEN_STRING){
                    buffer.data[buffer.writeIdxPtr][stringPtr++] = character;
                } else{
                    printf("Zeichenfolgenlänge überschreitet \n");
                }
            }
        }
    }else{
        printf("File not exist\n");
    }

    if(fclose(input)){
        printf("Closing Error");
    }
}

void getData(int32_t* data, uint16_t* dataLen){
    uint8_t checkEmpty = (!buffer.fullFlag) && (buffer.writeIdxPtr==buffer.readIdxPtr);
    uint16_t avaDataLen = getAvaDataLen();
    if(!checkEmpty){
        (*dataLen) = (uint16_t)min((*dataLen),avaDataLen);
    }
}




