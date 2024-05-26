/**
  ******************************************************************************
  * @file           : led.h
  * @brief          : Header for led.c file.
  *                   This file defines of the LED.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Exported types ------------------------------------------------------------*/

// 定义 LED 控制结构体
typedef struct{
    void (*On)(GPIO_TypeDef *, uint16_t);  // 点亮 LED 的函数指针
    void (*Off)(GPIO_TypeDef *, uint16_t); // 熄灭 LED 的函数指针
    void (*Toggle)(GPIO_TypeDef *, uint16_t); // 翻转 LED 的函数指针
    void (*Loop)(GPIO_TypeDef *, uint16_t, uint16_t); // 循环点亮与熄灭 LED 的函数指针
    uint8_t (*Status)(GPIO_TypeDef *, uint16_t); // 返回 LED 的状态函数指针
} LED_Control;

/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/

// LED 点亮函数
void LED_On(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin);

// LED 熄灭函数
void LED_Off(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin);


// 翻转 LED 的函数
void LED_Toggle(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin);


// 循环点亮与熄灭 LED 的函数
void LED_Loop(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin, uint16_t Delay);


// 检测 LED 状态的函数
uint8_t LED_Status(GPIO_TypeDef *GPIOx, uint16_t LEDx_Pin);

/* defines -----------------------------------------------------------*/

// 可以使用 LED_Init 代替 开发板名称初始化
#define SoraStar_LED_Init LED_Init

#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __LED_H */
