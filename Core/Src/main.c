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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "led.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PERIOD 20
#define ADCSYS 2
#define ADCIN 1
#define REC_BUF_LEN 64
#define HEADER_H 0x73 // 's'
#define HEADER_L 0x63 // 'c'
#define ENDER_H 0x78  // 'x'
#define ENDER_L 0x79  // 'y'

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t sysTSec = 0;
uint8_t sysVarFlag = 0;

extern DMA_HandleTypeDef hdma_usart1_rx;
UART_msg dmaRx = {0};

uint8_t rec_byte;
uint8_t rec_index = 0;
uint8_t rec_data[REC_BUF_LEN] = {0};

uint32_t ADC_originValue[ADCSYS] = {0};
uint32_t ADC2_originValue[ADCIN] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t KEY_Read(uint16_t Period);
void KEY_Process(void);
void UART_Process(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 创建 LED 结构体
LED_Control LED = {
    .On = LED_On,
    .Off = LED_Off,
    .Toggle = LED_Toggle,
    .Loop = LED_Loop,
    .Status = LED_Status};

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM12_Init();
  MX_ADC2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  // 开启 UART DMA接收

  if (HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)&dmaRx.data, REC_BUF_LEN) != HAL_OK)
  {
    Error_Handler();
  }

  // 开启 ADC_DMA 内部Temperature与Vref传感器通道转换
  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_originValue, 2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&ADC2_originValue, 1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // if (sysTSec > 2)
    // {
    //   // uint8_t ErrorBuf;
    //   sysTSec = 0;
    //   // HAL_UART_Transmit(&huart1, (uint8_t *)&dmaRx.len, 1, 0xFF);
    //   // for (int i = 0; i < 6; i++)
    //   // {
    //   //   HAL_UART_Transmit(&huart1, (uint8_t *)&dmaRx.data[i], 1, 0xFF);
    //   // }
    //   // ErrorBuf = HAL_UART_GetError(&huart1);
    //   // HAL_UART_Transmit(&huart1, (uint8_t *)&ErrorBuf, 1, 0xFF);

    // }

    KEY_Process();
    UART_Process();

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint8_t KEY_Read(uint16_t Period)
{
  static uint8_t Press_Flag = 0;
  static uint16_t Press_Delay = 0;

  if ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1) && Press_Flag == 0)
  {
    Press_Delay += Period;
    if (Press_Delay <= Period)
      return 0;

    if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
    {
      Press_Flag = 1;
      if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)
        return 1;
    }
  }

  if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0)
  {
    Press_Flag = 0;
    Press_Delay = 0;
  }

  return 0;
}

void KEY_Process(void)
{
  static uint8_t Key_Val;

  Key_Val = KEY_Read(PERIOD);
  if (Key_Val == 1)
  {
    HAL_UART_Transmit(&huart1, (uint8_t *)"K1Press\r\n", 10, 250);
    // LED.Toggle(GPIOB, LED1_Pin);
  }
}

void UART_Process(void)
{
  if (dmaRx.len >= 4 && (dmaRx.data[dmaRx.len - 2] == ENDER_H && dmaRx.data[dmaRx.len - 1] == ENDER_L))
  {
    if ((dmaRx.data[0] == HEADER_H && dmaRx.data[1] == HEADER_L)) // 开头正确
    {
      uint8_t LED_State;
      char temBuf[16];
      float tempValue;

      switch (dmaRx.data[2])
      {
      case 'N':
        LED.On(GPIOB, LED1_Pin);
        break;
      case 'F':
        LED.Off(GPIOB, LED1_Pin);
        break;
      case 'T':
        LED.Toggle(GPIOB, LED1_Pin);
        break;
      }

      LED_State = LED.Status(GPIOB, LED1_Pin);

      switch (dmaRx.data[3])
      {
      case 'L':
        if (LED_State == 1)
        {
          HAL_UART_Transmit(&huart1, (uint8_t *)"LED1 Status:On\r\n", 18, 0xFF);
        }
        else if (LED_State == 0)
        {
          HAL_UART_Transmit(&huart1, (uint8_t *)"LED1 Status:Off\r\n", 18, 0xFF);
        }
        break;
      case 'T':
        tempValue = ((ADC_originValue[0] / 4095.0) * 3300 - 760) / 2.5 + 25; // 根据ADC值计算温度
        sprintf(temBuf, "Temp:%.1f\r\n", tempValue);
        HAL_UART_Transmit(&huart1, (uint8_t *)temBuf, 12, 0xFF);
        break;
      case 'V':
        tempValue = ((ADC_originValue[1] / 4095.0) * 3.3); // 根据 ADC 值计算VrefIn片内参照电压
        sprintf(temBuf, "VREFINT:%.2fV\r\n", tempValue);
        HAL_UART_Transmit(&huart1, (uint8_t *)temBuf, 16, 0xFF);
        break;
      case 'A':
        tempValue = ((ADC2_originValue[0] / 4095.0) * 3.3); // 根据 ADC 值计算GPIO获取电压
        sprintf(temBuf, "ADCIN:%.2fV\r\n", tempValue);
        // tempValue = ((ADC2_originValue[0] / 4095.0) * 3.3);
        // sprintf(temBuf, "PC05=%.2f\r\n", tempValue);
        HAL_UART_Transmit(&huart1, (uint8_t *)temBuf, 14, 0xFF);
      }

      if (dmaRx.data[2] == '?' || dmaRx.data[3] == '?')
      {
        HAL_UART_Transmit(&huart1, (uint8_t *)"Help:sc??xy\r\n", 12, 0xFF);
        HAL_UART_Transmit(&huart1, (uint8_t *)"'s''c'..'x''y'", 15, 0xFF);
        HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n.1:LED:N/O/T", 16, 0xFF);
        HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n.2:L/T/V", 11, 0xFF);
      }

      // 本次数据结束，重新接收
      dmaRx.len = 0;
    }
    else // 开头错误，重新接收
    {
      dmaRx.len = 0;
    }
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART1)
  {
    // HAL_UART_Transmit(&huart1, (uint8_t *)dmaRx.data, Size, 0xFF);
    dmaRx.len = Size;
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (HAL_UART_GetError(huart) & HAL_UART_ERROR_PE)
  { /* Parity error */
    // 奇偶校验错误
    __HAL_UART_CLEAR_PEFLAG(huart);
  }
  else if (HAL_UART_GetError(huart) & HAL_UART_ERROR_NE)
  { /* Noise error */
    // 噪声错误
    __HAL_UART_CLEAR_NEFLAG(huart);
  }
  else if (HAL_UART_GetError(huart) & HAL_UART_ERROR_FE)
  { /* Frame error */
    // 帧格式错误
    __HAL_UART_CLEAR_FEFLAG(huart);
  }
  else if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE)
  { /* Overrun error */
    // 数据太多串口来不及接收错误
    __HAL_UART_CLEAR_OREFLAG(huart);
  }
  // 当这个串口发生了错误需要要重新使能接收中断
  if (huart->Instance == USART1)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)&dmaRx.data, REC_BUF_LEN);
  }
}

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

#ifdef USE_FULL_ASSERT
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
