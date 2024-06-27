#ifndef __GPIO_H_
#define __GPIO_H_

#include "stm32f4xx.h"

/*************LED引脚分布**************
	LED1--PE2--推挽输出--低电平点亮
	LED2--PE3--推挽输出
***********************************/
#define LED1_PIN 			GPIO_Pin_2
#define LED1_PORT 			GPIOE
#define LED1_CLK 			RCC_AHB1Periph_GPIOE

#define LED2_PIN 			GPIO_Pin_3
#define LED2_PORT 			GPIOE
#define LED2_CLK 			RCC_AHB1Periph_GPIOE

#define LED1_ON()			GPIO_ResetBits(LED1_PORT, LED1_PIN)
#define LED1_OFF()			GPIO_SetBits(LED1_PORT, LED1_PIN)


#define LED2_ON()			GPIO_ResetBits(LED2_PORT, LED2_PIN)
#define LED2_OFF()		    GPIO_SetBits(LED2_PORT, LED2_PIN)

void Led_Init(void);



/*************RGB引脚分布**************
	R_LED--PF6--推挽输出
	G_LED--PE9--推挽输出
	B_LED--PD15--推挽输出
***********************************/
/*
	0 0 0	BLACK
	0 0 1	BLUE
	0 1 0	GREEN
	0 1 1	Cyan
	1 0 0	RED
	1 0 1	PURPLE
	1 1 0	YELLOW
	1 1 1	WHITE
*/
typedef enum {
	BLACK 	= 0x00,	//黑色	0 0 0
	BLUE 	= 0x01,	//蓝色	0 0 1
	GREEN	= 0x02,	//绿色	0 1 0
	Cyan	= 0x03,	//青色	0 1 1
	RED 	= 0x04,	//红色	1 0 0
	PURPLE	= 0x05,	//紫色	1 0 1
	YELLOW	= 0x06,	//黄色	1 1 0
	WHITE	= 0x07,	//白色 	1 1 1
}color_t;


#define R_LED_PIN 			GPIO_Pin_6
#define R_LED_PORT 			GPIOF
#define R_LED_CLK 			RCC_AHB1Periph_GPIOF

#define G_LED_PIN 			GPIO_Pin_9
#define G_LED_PORT 			GPIOE
#define G_LED_CLK 			RCC_AHB1Periph_GPIOE

#define B_LED_PIN 			GPIO_Pin_15
#define B_LED_PORT 			GPIOD
#define B_LED_CLK 			RCC_AHB1Periph_GPIOD



#define R_F GPIO_ResetBits(R_LED_PORT, R_LED_PIN)
#define G_F GPIO_ResetBits(G_LED_PORT, G_LED_PIN)
#define B_F GPIO_ResetBits(B_LED_PORT, B_LED_PIN)
#define R_N GPIO_SetBits(R_LED_PORT, R_LED_PIN)
#define G_N GPIO_SetBits(G_LED_PORT, G_LED_PIN)
#define B_N GPIO_SetBits(B_LED_PORT, B_LED_PIN)

void RGBLed_Init(void);
void RGBLed_Control(color_t color);


/*************按键引脚分布**************
	KEY1--PB8--浮空输入
	KEY2--PB9--浮空输入
***********************************/

#define KEY1_PIN 			GPIO_Pin_8
#define KEY1_PORT 			GPIOB
#define KEY1_CLK 			RCC_AHB1Periph_GPIOB

#define KEY2_PIN 			GPIO_Pin_9
#define KEY2_PORT 			GPIOB
#define KEY2_CLK 			RCC_AHB1Periph_GPIOB

#define KEY_RELEASE			1
#define KEY_PRESS			0
#define KEY1_IN()			GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN)
#define KEY2_IN()			GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN)

void Key_Init(void);
uint8_t Key_Scan(void);

#endif


