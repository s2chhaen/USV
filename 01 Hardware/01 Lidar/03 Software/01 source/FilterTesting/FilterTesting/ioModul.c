#include "ioModul.h"

static ringBuffer4Str_t buffer = {{{0}},{0},0,0,0};

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
}




