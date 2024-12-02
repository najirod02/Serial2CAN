/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 25
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char serialBuffer[BUFFER_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;

//available only 3 mailboxes, the hw will automatically decide which
//to forward the frame
uint32_t TxMailBox[3];

//both to contain the maximum payload for both standard and extended frame
uint8_t TxData[8];
uint8_t RxData[8];

/**
 * when we have pending messages, read them, construct a string
 * to send back to uart to host pc.
 * 
 * in order to create a compliant slcand frame we have the following format:
 * <type><id><dlc><data>
 * 
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
  
  //copy Id and DLC in buffer
  char can2Uart[BUFFER_SIZE] = {0};
  char typeFrame = (RxHeader.RTR == CAN_RTR_DATA) ? 't' : 'r';
  sprintf(can2Uart, "%c%03lX%lu", typeFrame, RxHeader.StdId, RxHeader.DLC);

  //convertion of bytes into hex characters
  //1 byte = 2 hex values
  char *offset = can2Uart + strlen(can2Uart);
  for (uint8_t i = 0; i < RxHeader.DLC; i++) {
      sprintf(offset, "%02X", RxData[i]);
      offset += 2;
  }
  strcat(can2Uart, "\r");

  //an example of what should look like the final string
  //"t4563112233\r";// can_id 0x456, len 3, data 0x11 0x22 0x33

  HAL_UART_Transmit_IT(&huart2, (uint8_t *) can2Uart, strlen(can2Uart));
  memset(serialBuffer, 0, BUFFER_SIZE);//clear buffer
}

/**
 * when a can frame message is received from the serial, create a new frame to send
 * through the "real" can interface.
 * 
 * the frame created assumes a standard frame
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  //extended frames are dropped as they are not used
  if(serialBuffer[0] == 'T' || serialBuffer[0] == 'R') return;

  //create the can frame and send it to a mailbox
  char id[4] = {0};
  strncpy(id, &serialBuffer[1], 3);//assuming standard id
  TxHeader.StdId = (uint32_t)strtol(id, NULL, 16);

  TxHeader.ExtId = 0; //not using extended ID
  TxHeader.IDE = CAN_ID_STD; //standard ID

  TxHeader.RTR = (serialBuffer[0] == 't') ? 
                  CAN_RTR_DATA : CAN_RTR_REMOTE; //data frame
  
  TxHeader.TransmitGlobalTime = DISABLE;

  //collect payload to send skipping # char
  uint8_t payload_length = (strlen(serialBuffer) - 5) / 2; //each byte = 2 hex chars
  if (payload_length > 8) {
      //payload too large for standard can frame
      //drop message and abort sending can
      //for example, 123#001122334455667788 is dropped
      return;
  }

  if(TxHeader.RTR == CAN_RTR_DATA){
    for (int i = 0; i < payload_length; i++) {
      //create pair of hex chars
      char hex_byte[3] = {serialBuffer[5 + 2 * i], serialBuffer[5 + 2 * i + 1], '\0'};
      TxData[i] = (uint8_t)strtol(hex_byte, NULL, 16);//convert each hex pair to uint8_t
    }
    TxHeader.DLC = payload_length;
  } else {
    TxHeader.DLC = (uint32_t)serialBuffer[4];
  }

  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, &TxData[0], &TxMailBox[0]);

  //remain in listening for a new cansend frame
  memset(serialBuffer, 0, BUFFER_SIZE);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)serialBuffer, BUFFER_SIZE);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_CAN_Start(&hcan1);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //remain in listening for a cansend frame
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)serialBuffer, BUFFER_SIZE);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
