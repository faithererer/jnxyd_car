#ifndef _OLED_H
#define _OLED_H

#include "stm32f4xx.h"


/*************引脚分布**************
	OLED_CS----PF10
	OLED_DC----PF9
	OLED_RST---PF8
	OLED_SCLK--PA5--SPI1_SCLK
	OLED_MOSI--PA7--SPI1_MOSI
***********************************/

/*-----------------OLED端口定义-----------------*/
#define OLED_CS_PIN 			GPIO_Pin_10
#define OLED_CS_PORT 			GPIOF
#define OLED_CS_CLK 			RCC_AHB1Periph_GPIOF

#define OLED_DC_PIN 			GPIO_Pin_9
#define OLED_DC_PORT 			GPIOF
#define OLED_DC_CLK 			RCC_AHB1Periph_GPIOF

#define OLED_RST_PIN 			GPIO_Pin_8
#define OLED_RST_PORT 			GPIOF
#define OLED_RST_CLK 			RCC_AHB1Periph_GPIOF

#define OLED_SCLK_PIN 			GPIO_Pin_5
#define OLED_SCLK_PORT 			GPIOA
#define OLED_SCLK_CLK 			RCC_AHB1Periph_GPIOA
#define OLED_SCLK_PINSOURCE 	GPIO_PinSource5
#define OLED_SCLK_AF 			GPIO_AF_SPI1

#define OLED_MOSI_PIN 			GPIO_Pin_7
#define OLED_MOSI_PORT 			GPIOA
#define OLED_MOSI_CLK 			RCC_AHB1Periph_GPIOA
#define OLED_MOSI_PINSOURCE 	GPIO_PinSource7
#define OLED_MOSI_AF 			GPIO_AF_SPI1

#define OLED_SPI 				SPI1
#define OLED_SPI_CLK		 	RCC_APB2Periph_SPI1
#define OLED_SPI_CLK_INIT 		RCC_APB2PeriphClockCmd

#define OLED_CS_High()			do { GPIO_SetBits(OLED_CS_PORT, OLED_CS_PIN); } while(0)
#define OLED_CS_Low()			do { GPIO_ResetBits(OLED_CS_PORT, OLED_CS_PIN); } while(0)

#define OLED_DC_High()			do { GPIO_SetBits(OLED_DC_PORT, OLED_DC_PIN); } while(0)
#define OLED_DC_Low()			do { GPIO_ResetBits(OLED_DC_PORT, OLED_DC_PIN); } while(0)

#define OLED_RST_High()			do { GPIO_SetBits(OLED_RST_PORT, OLED_RST_PIN); } while(0)
#define OLED_RST_Low()			do { GPIO_ResetBits(OLED_RST_PORT, OLED_RST_PIN); } while(0)

void OLED_Init(void);
void OLED_Clear(void);
void OledDisplayString(char page,char colum,unsigned char *str);
void OledDisplayChinese(char page,char colum,unsigned char *font);

void oled_setpos(u8 x,u8 y);
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,u8 BMP[]);
#endif
