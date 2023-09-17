#include "ioModul.h"

static ringBuffer4Str_t buffer = {{{0}},{0},0,0,0};

static uint16_t getAvaDataLen(){
    uint16_t result = 0;
    int32_t temp = 0;
    if(buffer.fullFlag){
        result = 512;
    } else{
        temp = (buffer.writeIdxPtr - buffer.readIdxPtr)%BUFFER_MAX_LENGTH;
        result = (uint16_t) temp;
    }
    return result;
}

static inline uint8_t char2Num(char input){
    return input - 48;
}

static inline int32_t max(int32_t a, int32_t b){
    return (a>b)?a:b;
}

static inline int32_t min(int32_t a, int32_t b){
    return (a<b)?a:b;
}

static int32_t string2Num(uint8_t* input, uint8_t strLen){
    int32_t output = 0;
    //TODO Fehlerfall trennen
    uint8_t sign = 0;
    uint8_t beginIdx = 0;
    uint8_t num = 0;
    strLen--;//da die letzte Zeichen von String ist '\n' kein Nummer
    if((input[0]=='-')||(input[0]=='+')){
        sign = (input[0]=='-')?1:0;
        beginIdx = 1;
    }
    for(unsigned int i = beginIdx; i< strLen; i++){
        num = char2Num(input[i]);
        output += (int32_t)num*pow(10,(uint8_t)(strLen-1-i));
    }
    output = (sign)?((-1)*output):output;
    return output;
}

void readFile(){
    FILE *input = fopen("input.txt","r");
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

#ifdef DEBUG_SEC_1
    //Testen
    printf("Inhalt von Buffer \n");
    uint16_t temp = getAvaDataLen();
    for(unsigned int i = 0;i<temp;i++){
        printf("Val[%d] = %s \n",i,buffer.data[i]);
    }
#endif // DEBUG_SEC_1
}

void getData(int32_t* data, uint16_t* dataLen){
    uint8_t checkEmpty = (!buffer.fullFlag) && (buffer.writeIdxPtr==buffer.readIdxPtr);
    uint16_t avaDataLen = getAvaDataLen();
    uint16_t temp = 0;
    if(!checkEmpty){
        (*dataLen) = (uint16_t)min((*dataLen),avaDataLen);
        for(int i = 0; i < (*dataLen); i++){
            temp = buffer.readIdxPtr;
            buffer.readIdxPtr++;
            data[i] = string2Num(buffer.data[temp],buffer.dataLen[temp]);
            if(buffer.readIdxPtr==buffer.writeIdxPtr){
                buffer.fullFlag = 0;
                break;
            }
        }
    }
}





