#include "main.h"
#include <stdio.h>
#include "debug.h"

TIM_HandleTypeDef htim6;
UART_HandleTypeDef hUARTPins;  	// PINS
UART_HandleTypeDef hUARTUSB;	// USB (ST-LINK)

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);

int main(void){
  HAL_Init();
  SystemClock_Config();
  //MX_GPIO_Init();
  MX_USART1_UART_Init(); 		// pins
  MX_USART2_UART_Init(); 		// usb (ST-LINK)
  MX_TIM6_Init();				// laeuft mit 1kHz
  initCom(hUARTPins);

  initDebug(hUARTUSB);

  // --- Reglerparameter initialisieren ---
  // Geschwindigkeits-Regler
  controller pidVelocity = {
		  .eold = 0,
		  .esum = 0,
		  .esum_limit = 100, // in m/s
		  .kd = 0,
		  .ki = 0.006,
		  .kp = 0.055,
		  .ta = 1,
		  .y_max = 0.999,
		  .y_min = -1
  };
  // Peilungs-/Kurswinkel-Regler
  controller pidBearing = {
		  .eold = 0,
		  .esum = 0,
		  .esum_limit = 300, // in grd
		  .kd = 0,
		  .ki = 0.00005,
		  .kp = 0.0003,
		  .ta = 1,
		  .y_max = 0.999,
		  .y_min = -1
  };

  // Deklaration Datenstruktur der lokalen Daten
  localData data = { 0 };

  uint32_t oldTimestamp = 0;
  float setpointBearing = 0;
  //uint16_t timer_val;
  //float ta = 1;
  HAL_TIM_Base_Start(&htim6);
  __HAL_TIM_SET_COUNTER(&htim6,0);


  // Hauptschleife
  while(1){
	  // Daten von Bus holen
	  status s = updateSensorBlock(&data);
	  if(s != STATUS_OK){
		  // TODO Fehlerbehandlung
		  sendStrToUSB("Error1\r", 7);
	  }
	  else{
		  s = updateFuehrungsgrBlock(&data);
		  if(s != STATUS_OK){
		  // TODO Fehlerbehandlung
		  sendStrToUSB("Error2\r", 7);
		  }
	  }

	  if(oldTimestamp != data.timestamp){
		  // ---------- ignorieren, nicht in Nutznung ----------
		  // Zeitdiff ermitteln (funktioniert auch bei Ueberlauf, aber nicht bei diff > 65s
		  //timer_val = __HAL_TIM_GET_COUNTER(&htim6) - timer_val; // Zeit in ms
		  //ta = (float)timer_val;
		  //ta /= 1000;
		  //pidBearing.ta = ta;
		  //pidVelocity.ta = ta;
		  // ---------- ignorieren, nicht in Nutznung ----------

		  // Schub Stellgr. berechnen
		  data.thrust = pid_controll(&pidVelocity, data.targetSpeed, data.currGPSSpeed);
		  if(data.thrust < 0) // TODO negative Geschwindigkeiten
			  data.thrust = 0;

		  // ---------- ignorieren, nicht in Nutznung ----------
		  // Sollwert Kurswinkel berechnen (mittels TP)
		  // coord_t tp = calcTargetpoint(data.pointA, data.pointB, data.currPos);
		  //setpointBearing = calcBearing(data.currPos, tp);
		  // ---------- ignorieren, nicht in Nutznung ----------

		  // Sollwert Kurswinkel berechnen (ohne TP)
		  setpointBearing = calcBearing(data.currPos, data.pointB);

		  // Rudder Stellgr. berechnen
		  data.rudder = pid_controll(&pidBearing, data.currHeadingAngle, setpointBearing);

		  // Stellgr. auf Bus schreiben
		  if(updateStellgrBlock(&data) != STATUS_OK){
			  // TODO Fehlerbehandlung
		  }

	  }
	  oldTimestamp = data.timestamp;

	  // Warten bis 1 sek seit letztem ruecksetzen vergangen
	  while(__HAL_TIM_GET_COUNTER(&htim6) < 1000);
	  // Timer ruecksetzen
	  __HAL_TIM_SET_COUNTER(&htim6,0);
  }
}














// LED Test
//	  HAL_GPIO_WritePin(GPIOA, LED1_Pin, !(leds & 1));
//	  HAL_GPIO_WritePin(GPIOA, LED2_Pin, !(leds & 2));
//	  HAL_GPIO_WritePin(GPIOA, LED3_Pin, !(leds & 4));
//	  HAL_GPIO_WritePin(GPIOA, LED4_Pin, !(leds & 8));
//	  c++;
//	  if(c >= 150){
//		  leds = ~leds;
//		  c = 0;
//	  }


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 2000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  hUARTPins.Instance = USART1;
  hUARTPins.Init.BaudRate = 250000;
  hUARTPins.Init.WordLength = UART_WORDLENGTH_9B;
  hUARTPins.Init.StopBits = UART_STOPBITS_1;
  hUARTPins.Init.Parity = UART_PARITY_ODD;
  hUARTPins.Init.Mode = UART_MODE_TX_RX;
  hUARTPins.Init.HwFlowCtl = UART_HWCONTROL_NONE; // UART_HWCONTROL_RTS_CTS;
  hUARTPins.Init.OverSampling = UART_OVERSAMPLING_16;
  hUARTPins.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hUARTPins.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hUARTPins.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hUARTPins) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hUARTPins, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hUARTPins, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hUARTPins) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  hUARTUSB.Instance = USART2;
  hUARTUSB.Init.BaudRate = 115200;
  hUARTUSB.Init.WordLength = UART_WORDLENGTH_8B;
  hUARTUSB.Init.StopBits = UART_STOPBITS_1;
  hUARTUSB.Init.Parity = UART_PARITY_NONE;
  hUARTUSB.Init.Mode = UART_MODE_TX_RX;
  hUARTUSB.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hUARTUSB.Init.OverSampling = UART_OVERSAMPLING_16;
  hUARTUSB.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hUARTUSB.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hUARTUSB.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hUARTUSB) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hUARTUSB, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hUARTUSB, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hUARTUSB) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin LED4_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
