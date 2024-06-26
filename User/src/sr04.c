#include "sr04.h"
#include "timer.h"

static void Sr04_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	/* 使能 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(SR04_TRIG_CLK|SR04_ECHO_CLK, ENABLE);

	/* 配置 SR04_TRIG 引脚: 普通推挽输出 */
	GPIO_InitStructure.GPIO_Pin = SR04_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(SR04_TRIG_PORT, &GPIO_InitStructure);
	
	/* 配置 SR04_ECHO 引脚: 普通下拉输入 */
	GPIO_InitStructure.GPIO_Pin = SR04_ECHO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(SR04_ECHO_PORT, &GPIO_InitStructure);
}

static void Sr04_TIMInit(void)
{
	/* 定时器TIM2初始化 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	/* 使能定时器TIM2时钟 */
	SR04_TIM_CLK_INIT(SR04_TIM_CLK, ENABLE);
	TIM_DeInit(SR04_TIM);
	TIM_TimeBaseInitStruct.TIM_Period=SR04_TIM_ARR;				//定时周期为23200(0.58*40000)  
	TIM_TimeBaseInitStruct.TIM_Prescaler=SR04_TIM_PSC; 			//分频系数为83,即计数周期为1us
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;		//不分频
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(SR04_TIM, &TIM_TimeBaseInitStruct);
	TIM_Cmd(SR04_TIM, DISABLE);//关闭定时器使能  
	TIM_ClearFlag(SR04_TIM, TIM_FLAG_Update);	
}
	
void Sr04_Init(void)
{
	Sr04_GPIOInit();
	Sr04_TIMInit();
}

/*
	* 函数名称   ： Sr04_Measure
	* 函数功能   ： 获取测试距离
	* 函数形参   ： 无
	* 函数返回值 ： 成功返回测试距离,超出测试距离返回-1,线路连接异常返回-2
	* 函数描述   ： 建议测量周期为 60ms 以上, 以防止发射信号对回响信号的影响
*/
float Sr04_Measure(void)
{
	float distance;
	uint32_t time = 20000;
	/* 产生触发信号 */
	SR04_TRIG_Low();
	SR04_TRIG_High();				
	delay_us(20);					
	SR04_TRIG_Low();
	/* 检测回响信号高电平持续时间 */
	while ( !SRO4_ECHO_IN() ) 
	{		//等待回响信号变为高电平
		if ( ! time-- )
			return -2;				//线路连接异常
	}
	TIM_SetCounter(SR04_TIM, 0);		//清空定时器计数器
	TIM_Cmd(SR04_TIM, ENABLE);			//启动定时器
	while ( SRO4_ECHO_IN() ) 
	{		//等待回响信号变为低电平
		if ( TIM_GetFlagStatus(SR04_TIM, TIM_FLAG_Update) ) 
		{	
			TIM_ClearFlag(SR04_TIM, TIM_FLAG_Update);
			TIM_Cmd(SR04_TIM, DISABLE);	
			return -1;				//超出测量范围(2cm~400cm)
		}
	}		
	TIM_Cmd(SR04_TIM, DISABLE);			//关闭定时器
	time = TIM_GetCounter(SR04_TIM);	//获取定时器计数器值,即回响信号高电平时间us
	/* 计算测试距离 */
	distance = time / 58.0;
	return distance;
}
