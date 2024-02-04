/*
 * usvDataHandler.c
 *
 * Created: 7/7/2023 8:42:59 AM
 * Author: Thach
 * Version: 1.4
 * Revision: 1.2
 */

#include "usvDataHandler.h"

//Speichern der Max. Elementen vom USART-FIFO
const uint16_t usartFIFOMaxLen = _FIFO_max_def - 1;
//Protocolbereich
volatile uint8_t protocol[MAX_FRAME_LEN] = {0};
volatile uint8_t usv_protocolToHandleBytes = 0;
volatile uint8_t usv_protocolIdx = 0;
//Temp-Buffer zum Speichern der noch nicht gesendeten Daten
volatile uint8_t usv_tempBuffer[370] = {0};
volatile uint16_t usv_tempBufferToHandleBytes = 0;
volatile uint16_t usv_tempBufferIdx = 0; 
//Zeiger zu den externen Daten and dem Status-Register
static const uint8_t* usv_dataBuffer = NULL;
static const uint16_t* usv_dataBufferLen = NULL;
static const uint8_t* usv_statusBuffer = NULL;
static const uint8_t* usv_statusBufferLen = NULL;

//Verwaltungsbereich
volatile usvMgr_t usv_mgr = {0};
volatile usartConfig_t usv_comParam = {0};
volatile uint8_t usv_checksumPolynom = 0;
volatile uint16_t usv_nextReg = 0;
volatile uint8_t usv_savedAddr = 0;
volatile reg8Model_t* usv_ioStream = NULL;
static int8_t usv_tryTime = USV_RETRY_TIME_MAX;
static uint8_t usv_errorSrc = USV_NO_SRC;
volatile uint8_t usv_programPos = COM_PROGRAMM_NORMAL_POS; 






