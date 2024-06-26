#ifndef __BC7210_H_
#define __BC7210_H_

#include "stm32f4xx.h"

typedef struct {
	uint8_t ir_buff[3];		//���������������
	uint8_t ir_len;			//����������ݳ���
	uint8_t ir_flag;		//�������������ɱ�־
} infrared_t;

typedef enum {
	ROOF_STOP = 0,			//ֹͣ
	ROOF_UP = 1,			//����
	ROOF_DOWN = 2,			//����
	ROOF_LEFT = 3,			//����
	ROOF_RIGHT = 4,			//����
	ROOF_DIRECTION = 5,		//ָ������
} roofaction_t;				//���峵������

typedef enum {
	BODY_STOP = 0,			//ֹͣ
	BODY_FORWARD = 1,		//ǰ��
	BODY_BACKWARD = 2,		//����
	BODY_MOVELEFT = 3,		//����
	BODY_MOVERIGHT = 4,		//����
	BODY_TURNLEFT = 5,		//��ת
	BODY_TURNRIGHT = 6,		//��ת
	BODY_SPINLEFT = 7,		//����
	BODY_SPINRIGHT = 8,		//����
	BODY_STEPBACK = 9,		//����һ��
} bodyaction_t;				//���峵����

/*************���ŷֲ�**************
	IR_SS--PA15--SPI1_NSS--���ø�������
	IR_DAT--PB5--SPI1_MOSI-���ø�������
	IR_SCK--PB3--SPI1_SCK--���ø�������
	IR_ACT--PB4--SPI1_MISO-������������
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
