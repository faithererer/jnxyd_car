#include "gpio.h"
#include "timer.h"

/*
�������ƣ�Led_Init
�������ܣ�LED��ʼ��
����    ����
����ֵ  ����
��ע    �� LED1---PE2   �������
           LED2---PE3
*/
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* �� Led ʱ�� */
	RCC_AHB1PeriphClockCmd(LED1_CLK|LED2_CLK,ENABLE);

	/* ��ʼ��LED1 */
	GPIO_InitStruct.GPIO_Pin = LED1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_PORT, &GPIO_InitStruct);
	
	/* ��ʼ��LED2 */
	GPIO_InitStruct.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStruct);	
	LED1_OFF();
	LED2_OFF();
}


void RGBLed_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* �� Led ʱ�� */
	RCC_AHB1PeriphClockCmd(R_LED_CLK|G_LED_CLK|B_LED_CLK,ENABLE);

	/* ��ʼ��R_LED */
	GPIO_InitStruct.GPIO_Pin = R_LED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(R_LED_PORT, &GPIO_InitStruct);
	
	/* ��ʼ��G_LED */
	GPIO_InitStruct.GPIO_Pin = G_LED_PIN;
	GPIO_Init(G_LED_PORT, &GPIO_InitStruct);	
	
	/* ��ʼ��B_LED */
	GPIO_InitStruct.GPIO_Pin = B_LED_PIN;
	GPIO_Init(B_LED_PORT, &GPIO_InitStruct);	

	GPIO_ResetBits(R_LED_PORT, R_LED_PIN);
	GPIO_ResetBits(G_LED_PORT, G_LED_PIN);
	GPIO_SetBits(B_LED_PORT, B_LED_PIN);
}


void RGBLed_Control(color_t color)
{
	switch (color) 
	{
		case BLACK:R_F;G_F;B_F;break;
		case BLUE:R_F;G_F;B_N;break;
		case GREEN:R_F;G_N;B_F;break;
		case Cyan:R_F;G_N;B_N;break;
		case RED:R_N;G_F;B_F;break;
		case PURPLE:R_N;G_F;B_N;break;
		case YELLOW:R_N;G_N;B_F;break;
		case WHITE:R_N;G_N;B_N;break;
	}
}

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* �� Key ʱ�� */
	RCC_AHB1PeriphClockCmd(KEY1_CLK|KEY2_CLK,ENABLE);
	
	/* ��ʼ��KEY1 */
	GPIO_InitStruct.GPIO_Pin = KEY1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	/* ��ʼ��KEY2 */
	GPIO_InitStruct.GPIO_Pin = KEY2_PIN;
	GPIO_Init(KEY2_PORT, &GPIO_InitStruct);
}



/*
	* ��������   �� Key_Scan
	* ��������   �� ����ɨ�躯��
	* �����β�   �� ��
	* ��������ֵ �� ����1���·���1,����2���·���2,���򷵻�0
	* ��������   �� ��
*/
uint8_t Key_Scan(void)
{
	uint8_t key_value = 0;
	static uint8_t key_flag = 0;	//�������±�־
	switch (key_flag) 
	{
		//����������ʱ
		case 0:
			if (KEY1_IN()==KEY_PRESS || KEY2_IN()==KEY_PRESS) 
			{
				delay_ms(10);
				if (KEY1_IN()==KEY_PRESS) 
				{
					key_value = 1;
					key_flag = 1;
				}
				if (KEY2_IN()==KEY_PRESS) 
				{
					key_value = 2;
					key_flag = 1;
				}
			}
			break;
		//����������ʱ
		case 1:
			if (KEY1_IN()==KEY_RELEASE && KEY2_IN()==KEY_RELEASE) 
			{
				key_flag = 0;
			}
			break;
	}
	return key_value;
}
