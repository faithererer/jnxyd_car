#ifndef __TIMER_H__
#define __TIMER_H__
#include <stm32f4xx.h>
#include "stm32f4xx_conf.h"

#define PSC_MS 8399
#define PSC_US 83

#define delay_init()	TIM6_Delay_Init(void)
#define delay_ms(time)	Tim6_Delay_ms(time)
#define delay_us(time)	Tim6_Delay_us(time)



void TIM6_Delay_Init(void);
void Tim6_Delay_ms(u16 ms);
void Tim6_Delay_us(u16 us);

#endif
