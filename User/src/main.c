#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"
#include "timer.h"
#include "motor.h"
#include "usart.h"
#include "voice.h"
#include "sr04.h"
#include "oled.h"
#include "pstwo.h"
#include "bc7210.h"
#include "pic.h"

int main(void)
{
//	float len;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Led_Init();
	RGBLed_Init();
	TIM6_Delay_Init();
	Key_Init();
 	Motor_Init();//电机初始化
	Motor_SetSpeed(3);
	usart_init(115200);
	Bc7210_Init();
//	Voice_Init();
	Voice_Play(0x003);
	OLED_Init();
	PS2_Init();

	// 快速显示一系列图片
    unsigned char* pictures[] = {pic1, pic1, pic1, pic1, pic1, pic1}; // 假设你有这些图片数据
    int num_pictures = 6; // 图片数量
    
    for (int i = 0; i < num_pictures; i++) {
        OLED_Clear();  // 清屏
        OLED_DrawBMP(0, 0, 64, 8, pictures[i]); // 显示图片
        delay_ms(200);  // 延迟200毫秒
    }

  // 显示六个人的名字
    char* names[] = {"张景超", "Bob", "Charlie", "David", "Eve", "Frank"};
    for (int i = 0; i < 6; i++) {
        OLED_Clear();  // 清屏
        OledDisplayString(0, 0, (unsigned char*)names[i]); // 显示名字
        delay_ms(1000);  // 延迟1秒
    }


	while (1) 
	{
//		Tim6_Delay_ms(30);
		PstwoControl();
		InfraredControl();
	}
}


/*
宏用于函数的参数检查
如果expr为false，则调用assert_failed函数报告源文件的名称和失败调用的源行号
如果expr为true，则不返回任何值。
*/
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	printf("%s: file %s on line %d\r\n", __FUNCTION__, file, line);
	abort();      //终止程序
}
#endif

void HardFault_Handler(void)
{
	printf("Hard Error!\r\n");
	while (1);
}
