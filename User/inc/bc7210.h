#ifndef __BC7210_H_
#define __BC7210_H_

#include "stm32f4xx.h"

typedef struct {
	uint8_t ir_buff[3];		//红外接收数据区域
	uint8_t ir_len;			//红外接收数据长度
	uint8_t ir_flag;		//红外接收数据完成标志
} infrared_t;

typedef enum {
	ROOF_STOP = 0,			//停止
	ROOF_UP = 1,			//向上
	ROOF_DOWN = 2,			//向下
	ROOF_LEFT = 3,			//向左
	ROOF_RIGHT = 4,			//向右
	ROOF_DIRECTION = 5,		//指定方向
} roofaction_t;				//定义车顶动作

typedef enum {
	BODY_STOP = 0,			//停止
	BODY_FORWARD = 1,		//前进
	BODY_BACKWARD = 2,		//后退
	BODY_MOVELEFT = 3,		//左移
	BODY_MOVERIGHT = 4,		//右移
	BODY_TURNLEFT = 5,		//左转
	BODY_TURNRIGHT = 6,		//右转
	BODY_SPINLEFT = 7,		//左旋
	BODY_SPINRIGHT = 8,		//右旋
	BODY_STEPBACK = 9,		//后退一步
} bodyaction_t;				//定义车身动作

/*************引脚分布**************
	IR_SS--PA15--SPI1_NSS--复用浮空输入
	IR_DAT--PB5--SPI1_MOSI-复用浮空输入
	IR_SCK--PB3--SPI1_SCK--复用浮空输入
	IR_ACT--PB4--SPI1_MISO-上拉浮空输入
***********************************/

#define IR_SS_PIN 			GPIO_Pin_15
#define IR_SS_PORT 			GPIOA
#define IR_SS_CLK 			RCC_AHB1Periph_GPIOA
#define IR_SS_PINSOURCE 	GPIO_PinSource15
#define IR_SS_AF 			GPIO_AF_SPI3

#define IR_DAT_PIN 			GPIO_Pin_5
#define IR_DAT_PORT 		GPIOB
#define IR_DAT_CLK 			RCC_AHB1Periph_GPIOB
#define IR_DAT_PINSOURCE 	GPIO_PinSource5
#define IR_DAT_AF 			GPIO_AF_SPI3

#define IR_SCK_PIN 			GPIO_Pin_3
#define IR_SCK_PORT 		GPIOB
#define IR_SCK_CLK 			RCC_AHB1Periph_GPIOB
#define IR_SCK_PINSOURCE 	GPIO_PinSource3
#define IR_SCK_AF 			GPIO_AF_SPI3

#define IR_ACT_PIN 			GPIO_Pin_4
#define IR_ACT_PORT 		GPIOB
#define IR_ACT_CLK 			RCC_AHB1Periph_GPIOB

#define IR_SPI 				SPI3
#define IR_SPI_CLK			RCC_APB1Periph_SPI3
#define IR_SPI_INIT 		RCC_APB1PeriphClockCmd
#define IR_SPI_IRQn 		SPI3_IRQn
#define IR_SPI_IRQHandler 	SPI3_IRQHandler

#define IR_ACT_IN()			GPIO_ReadInputDataBit(IR_ACT_PORT, IR_ACT_PIN)

#define Infrared_Init()		Bc7210_Init()

extern infrared_t ir_data;


void Bc7210_Init(void);
uint8_t IR_DataKey(void);
void InfraredControl(void);

#endif
