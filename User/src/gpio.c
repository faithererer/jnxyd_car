#include "gpio.h"
#include "timer.h"

/*
函数名称：Led_Init
函数功能：LED初始化
参数    ：无
返回值  ：无
备注    ： LED1---PE2   推挽输出
           LED2---PE3
*/
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* 打开 Led 时钟 */
	RCC_AHB1PeriphClockCmd(LED1_CLK|LED2_CLK,ENABLE);

	/* 初始化LED1 */
	GPIO_InitStruct.GPIO_Pin = LED1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_PORT, &GPIO_InitStruct);
	
	/* 初始化LED2 */
	GPIO_InitStruct.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStruct);	
	LED1_OFF();
	LED2_OFF();
}


void RGBLed_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* 打开 Led 时钟 */
	RCC_AHB1PeriphClockCmd(R_LED_CLK|G_LED_CLK|B_LED_CLK,ENABLE);

	/* 初始化R_LED */
	GPIO_InitStruct.GPIO_Pin = R_LED_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(R_LED_PORT, &GPIO_InitStruct);
	
	/* 初始化G_LED */
	GPIO_InitStruct.GPIO_Pin = G_LED_PIN;
	GPIO_Init(G_LED_PORT, &GPIO_InitStruct);	
	
	/* 初始化B_LED */
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
	/* 打开 Key 时钟 */
	RCC_AHB1PeriphClockCmd(KEY1_CLK|KEY2_CLK,ENABLE);
	
	/* 初始化KEY1 */
	GPIO_InitStruct.GPIO_Pin = KEY1_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	/* 初始化KEY2 */
	GPIO_InitStruct.GPIO_Pin = KEY2_PIN;
	GPIO_Init(KEY2_PORT, &GPIO_InitStruct);
}



/*
	* 函数名称   ： Key_Scan
	* 函数功能   ： 按键扫描函数
	* 函数形参   ： 无
	* 函数返回值 ： 按键1按下返回1,按键2按下返回2,否则返回0
	* 函数描述   ： 无
*/
uint8_t Key_Scan(void)
{
	uint8_t key_value = 0;
	static uint8_t key_flag = 0;	//按键按下标志
	switch (key_flag) 
	{
		//当按键按下时
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
		//当按键松手时
		case 1:
			if (KEY1_IN()==KEY_RELEASE && KEY2_IN()==KEY_RELEASE) 
			{
				key_flag = 0;
			}
			break;
	}
	return key_value;
}
