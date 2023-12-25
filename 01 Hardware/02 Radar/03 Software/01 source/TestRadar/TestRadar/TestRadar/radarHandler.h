/*
 * radarHandler.h
 *
 * Created: 12/12/2023 11:53:20 AM
 *  Author: Thach
 */ 


#ifndef RADARHANDLER_H_
#define RADARHANDLER_H_


//msg: message
#define RADAR_SYNC_MSG "{\"Reset\":\"Board was reset.\x20\x20\'?R\' returns details\x22}\r\n"
#define RADAR_DATA_REQ_CMD "PP"
#define RADAR_REQ_PROTOCOL_LEN (sizeof(RADAR_DATA_REQ_CMD)/sizeof(uint8_t) - 1)



#endif /* RADARHANDLER_H_ */