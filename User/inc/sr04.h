#ifndef __SR04_H_
#define __SR04_H_

#include "stm32f4xx.h"

/*************引脚分布**************
	SR04_TRIG--PC2--推挽输出
	SR04_ECHO--PC3--浮空输入
***********************************/

#define SR04_TRIG_PIN 			GPIO_Pin_2
#define SR04_TRIG_PORT 			GPIOC
#define SR04_TRIG_CLK 			RCC_AHB1Periph_GPIOC

#define SR04_ECHO_PIN 			GPIO_Pin_3
#define SR04_ECHO_PORT 			GPIOC
#define SR04_ECHO_CLK 			RCC_AHB1Periph_GPIOC

#define SR04_TIM 				TIM2
#define SR04_TIM_PSC			83	
#define SR04_TIM_ARR			24000   
#define SR04_TIM_CLK 			RCC_APB1Periph_TIM2
#define SR04_TIM_CLK_INIT		RCC_APB1PeriphClockCmd


#define SRO4_ECHO_IN()			GPIO_ReadInputDataBit(SR04_ECHO_PORT, SR04_ECHO_PIN)
#define SR04_TRIG_High()		GPIO_SetBits(SR04_TRIG_PORT, SR04_TRIG_PIN)
#define SR04_TRIG_Low()			GPIO_ResetBits(SR04_TRIG_PORT, SR04_TRIG_PIN)

void Sr04_Init(void);
float Sr04_Measure(void);

#endif

