#ifndef _PSTWO_H
#define _PSTWO_H

#include "stm32f4xx.h" 
#include "timer.h"
#include "motor.h"




/*************引脚分布**************
	PS2_CS----PB12
	PS2_SCLK--PB13
	PS2_MOSI--PB15
	PS2_MISO--PB14
***********************************/

#define PS2_CS_PIN 				GPIO_Pin_12
#define PS2_CS_PORT 			GPIOB
#define PS2_CS_CLK 				RCC_AHB1Periph_GPIOB

#define PS2_SCLK_PIN 			GPIO_Pin_13
#define PS2_SCLK_PORT 			GPIOB
#define PS2_SCLK_CLK 			RCC_AHB1Periph_GPIOB

#define PS2_MOSI_PIN 			GPIO_Pin_15
#define PS2_MOSI_PORT 			GPIOB
#define PS2_MOSI_CLK 			RCC_AHB1Periph_GPIOB

#define PS2_MISO_PIN 			GPIO_Pin_14
#define PS2_MISO_PORT 			GPIOB
#define PS2_MISO_CLK 			RCC_AHB1Periph_GPIOB


#define DI			GPIO_ReadInputDataBit(PS2_MISO_PORT, PS2_MISO_PIN)	//PB14  输入

#define DO_H 		GPIO_SetBits(PS2_MOSI_PORT, PS2_MOSI_PIN)		//命令位高
#define DO_L 		GPIO_ResetBits(PS2_MOSI_PORT, PS2_MOSI_PIN)		//命令位低

#define CS_H 		GPIO_SetBits(PS2_CS_PORT, PS2_CS_PIN)      		//CS拉高
#define CS_L 		GPIO_ResetBits(PS2_CS_PORT, PS2_CS_PIN)  		//CS拉低

#define CLK_H 		GPIO_SetBits(PS2_SCLK_PORT, PS2_SCLK_PIN)  		//时钟拉高
#define CLK_L 		GPIO_ResetBits(PS2_SCLK_PORT, PS2_SCLK_PIN)  	//时钟拉低

//These are our button constants
#define PSB_SELECT      1           
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4

#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8

#define PSB_L2         	9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12

#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      26

//#define WHAMMY_BAR		8

//These are stick values
#define PSS_RX 		5		//右摇杆X轴数据
#define PSS_RY 		6		//右摇杆y轴数据
#define PSS_LX 		7		//左摇杆X轴数据
#define PSS_LY 		8		//左摇杆X轴数据
void PS2_Init(void);
uint8_t PS2_RedLight(void);				//判断是否为红灯模式
uint8_t PS2_DataKey(void);		  		//键值读取
uint8_t PS2_AnologData(uint8_t button);	//得到一个摇杆的模拟量
void PstwoControl(void);
extern u8 err[50];

uint32_t sys_now(void);
void sys_now_init(void);
#endif





