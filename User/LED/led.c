#include "led.h"

void SoraStar_LED_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

}

// LED 点亮函数
void LED_On(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin)
{
  HAL_GPIO_WritePin(GPIOx, LEDx_Pin, GPIO_PIN_SET);
}

// LED 熄灭函数
void LED_Off(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin)
{
  HAL_GPIO_WritePin(GPIOx, LEDx_Pin, GPIO_PIN_RESET);
}

// 翻转 LED 的函数
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin)
{
  HAL_GPIO_TogglePin(GPIOx, LEDx_Pin);
}

// 循环点亮与熄灭 LED 的函数
void LED_Loop(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin, uint16_t Delay)
{
  HAL_GPIO_TogglePin(GPIOx, LEDx_Pin);
  HAL_Delay(Delay);
}

// 检测 LED 状态的函数
uint8_t LED_Status(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin)
{
  uint8_t Status;
  typedef enum
  {
    Off = 0,
    On = 1
  } LED_State;

  Status = HAL_GPIO_ReadPin(GPIOx, LEDx_Pin);
  if (Status == On)
  {
    return On;
  }
  else if (Status == Off)
  {
    return Off;
  }
  return Off;
}


