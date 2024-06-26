#include "timer.h"

/*
*注意： TIM_TimeBaseInitTypeDef 结构体里面有 5 个成员， TIM6 和 TIM7 的寄存器里面只有
*TIM_Prescaler 和 TIM_Period，所以使用 TIM6 和 TIM7 的时候只需初始化这两个成员即可，
* 另外三个成员是通用定时器和高级定时器才有.
*-----------------------------------------------------------------------
* TIM_Prescaler 都有
* TIM_CounterMode TIMx,x[6,7]没有，其他都有（基本定时器）
* TIM_Period 都有
* TIM_ClockDivision TIMx,x[6,7]没有，其他都有(基本定时器)
* TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
*-----------------------------------------------------------------------
*/


/*
* 函数名称  ： TIM6_Delay_Init
* 函数功能  ： 定时器6初始化
* 函数形参  ： 无
* 函数返回值： 无
* 函数描述  ： APB1 -- 84MHz
*/
void TIM6_Delay_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);         //使能定时器时钟
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;    //时钟分割
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);             //定时器初始化
	
	TIM_ARRPreloadConfig(TIM6,ENABLE);                          //使能自动重装载寄存器
	TIM_SelectOnePulseMode(TIM6,TIM_OPMode_Repetitive);         //循环计数
	TIM_UpdateRequestConfig(TIM6,TIM_UpdateSource_Global);
	TIM_UpdateDisableConfig(TIM6,DISABLE);                      //使能更新事件
	
	TIM_Cmd(TIM6,DISABLE);
}
/*
* 函数名称  ： Tim6_Delay_ms
* 函数功能  ： 定时器6毫秒级延时函数
* 函数形参  ： u16 ms
* 函数返回值： 无
* 函数描述  ： APB1 -- 84MHz
*/
void Tim6_Delay_ms(u16 ms)
{
	TIM_PrescalerConfig(TIM6,8400-1,TIM_PSCReloadMode_Update);   //设置预分频系数
	TIM_SetAutoreload(TIM6,10*ms);                                //重装载值
	
	TIM_GenerateEvent(TIM6,TIM_EventSource_Update);               //软件产生更新事件
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);                          //溢出标志位清零
	TIM_Cmd(TIM6,ENABLE);               
	
	while(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == RESET)
	{
		/* 等待时间到达 */
	}
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,DISABLE);
}
/*
* 函数名称  ： Tim6_Delay_us
* 函数功能  ： 定时器6微秒级延时函数
* 函数形参  ： u16 us
* 函数返回值： 无
* 函数描述  ： APB1 -- 84MHz
*/
void Tim6_Delay_us(u16 us)
{
	TIM_PrescalerConfig(TIM6,84-1,TIM_PSCReloadMode_Update);   //设置预分频系数
	TIM_SetAutoreload(TIM6,us);                                //重装载值
	
	TIM_GenerateEvent(TIM6,TIM_EventSource_Update);               //软件产生更新事件
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);                          //溢出标志位清零
	TIM_Cmd(TIM6,ENABLE);               
	
	while(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == RESET)
	{
		/* 等待时间到达 */
	}
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,DISABLE);
	
}


