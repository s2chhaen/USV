#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "pid_controller.h"
#include "calculation.h"
#include "conversion.h"
#include "communication.h"
//#include "test.h"

void Error_Handler(void);

#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_7
#define LED4_GPIO_Port GPIOA


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
