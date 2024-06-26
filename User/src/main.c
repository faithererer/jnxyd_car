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
 	Motor_Init();//�����ʼ��
	Motor_SetSpeed(3);
	usart_init(115200);
	Bc7210_Init();
//	Voice_Init();
//	Voice_Play(0x003);
	OLED_Init();
	PS2_Init();

  // ��ʾ�����˵�����
    char* names[] = {"Alice", "Bob", "Charlie", "David", "Eve", "Frank"};
    for (int i = 0; i < 6; i++) {
        OLED_Clear();  // ����
        OledDisplayString(0, 0, (unsigned char*)names[i]); // ��ʾ����
        Delay_ms(1000);  // �ӳ�1��
    }
	   // ������ʾһϵ��ͼƬ
    unsigned char* pictures[] = {pic1, pic1, pic1, pic1, pic1, pic1}; // ����������ЩͼƬ����
    int num_pictures = 6; // ͼƬ����
    
    for (int i = 0; i < num_pictures; i++) {
        OLED_Clear();  // ����
        OLED_DrawBMP(0, 0, 64, 8, pictures[i]); // ��ʾͼƬ
        Delay_ms(200);  // �ӳ�200����
    }

//	void OledDisplayString(char page,char colum,unsigned char *str)
//OledDisplayString(1,10,"����e��z���Ź�");
	OLED_DrawBMP(0,0,64,7,pic); //0,0λ����ʾ64*64ͼƬ
	while (1) 
	{
//		Tim6_Delay_ms(30);
		PstwoControl();
		InfraredControl();
	}
}


/*
�����ں����Ĳ������
���exprΪfalse�������assert_failed��������Դ�ļ������ƺ�ʧ�ܵ��õ�Դ�к�
���exprΪtrue���򲻷����κ�ֵ��
*/
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	printf("%s: file %s on line %d\r\n", __FUNCTION__, file, line);
	abort();      //��ֹ����
}
#endif

void HardFault_Handler(void)
{
	printf("Hard Error!\r\n");
	while (1);
}
