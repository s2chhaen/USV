#include "ioModul.h"

////Zeichenfolge-Array (Max. Länge der Zeichenfolge = 10)
//uint8_t dataBuffer[BUFFER_MAX_LENGTH][MAX_LEN_STRING] = {0};
//uint8_t dataLen[BUFFER_MAX_LENGTH] = {0};
//uint16_t readIdxPtr = 0;
//uint16_t writeIdxPtr = 0;
//uint8_t fullFlag = 0;

static ringBuffer4Str_t buffer = {{{0}},{0},0,0,0};

void readFile(){
    FILE *input = fopen("input.txt","r");
    //uint8_t checkReading = ferror(input);

    if(input!=NULL){
        uint16_t sizeBuffer = sizeof(buffer.data)/(sizeof(buffer.data[0]));
        printf("size of string Buffer = %" PRIu32 "\n",sizeBuffer);
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
                    printf("%c",character);
                }
            }else{
                if(stringPtr<MAX_LEN_STRING){
                    buffer.data[buffer.writeIdxPtr][stringPtr++] = character;
                    putchar(character);
                } else{
                    printf("Zeichenfolgenlänge überschreitet \n");
                }
            }
        }
//        while((letter = fgetc(input)) != EOF){
//            putchar(letter);
//        }
    }else{
        printf("File not exist\n");
    }

    if(fclose(input)){
        printf("Closing Error");
    }
}

