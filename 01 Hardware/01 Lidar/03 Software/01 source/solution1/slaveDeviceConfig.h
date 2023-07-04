/*
 * slaveDeviceConfig.h
 *
 * Created: 7/3/2023 10:04:33 PM
 *  Author: Dan
 */ 


#ifndef SLAVEDEVICECONFIG_H_
#define SLAVEDEVICECONFIG_H_

//#define SLAVE_STATIC 0
#define FULL_STRUCT 0
#define RX_BUFFER_LEN 900
#define TX_BUFFER_LEN 900
#define NO_OF_RX_BUFFER 4

enum fifoState{
	EMPTY,
	FILLED,
	FULL
};

#endif /* SLAVEDEVICECONFIG_H_ */