#include "stm32f4xx.h"
#include "motor.h"
#include "timer.h"
#include <stdio.h>

/**************直流电机引脚分布************
	左后 L_AIN1--PD8 		L_AIN2--PD9 		L_PWMA--PWM1--PD12--TIM4_CH1
	左前 L_BIN1--PD10		L_BIN2--PD11		L_PWMB--PWM2--PD13--TIM4_CH2
	右前 R_AIN1--PE15		R_AIN2--PE14		R_PWMA--PWM3--PB0---TIM3_CH3
	右后 R_BIN1--PE12		R_BIN2--PE11		R_PWMB--PWM4--PB1---TIM3_CH4

	左边轮使能 L_STBY--PD14
	右边轮使能 R_STBY--PE13
******************************************/
 
void Motor_Init(void)
{
	Motor_GPIOInit();
	Motor_PwmInit();
	//STBY使能	
	LSTBY_STATUS(ENABLE);
	RSTBY_STATUS(ENABLE);
}
 
 
void Motor_GPIOInit(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//开启时钟（GPIOB   GPIOD  GPIOE）
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	//模式配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;   //输出模式  
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;  //推挽输出：输出高低电平
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//输出速度
	//左后motor1 左前motor2
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	//右前motor3 右后motor4
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStruct);                
	//使能 -- STBY
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_14 ;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 ;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}
/*
	* 函数名称   ： MOTOR_PwmInit
	* 函数功能   ： 电机PWM初始化
	* 函数形参   ： 无
	* 函数返回值 ： 无
	* 函数描述   ： 无
L_PWMA--PWM1--PD12--TIM4_CH1
L_PWMB--PWM2--PD13--TIM4_CH2
R_PWMA--PWM3--PB0---TIM3_CH3
R_PWMB--PWM4--PB1---TIM3_CH4

定时器频率：84 000 000Hz   1秒有84 000 000 个脉冲
电机需要10KHz的频率： 那就是周期 = 1 / 10 000 = 0.1ms = 100us

分频      ：84           84 000 000 / 84 = 1 000 000
上限值    ：100  
           
84 * 100 = 84 00 
8400 / 84 000 000 = 1 / 10 000 = 0.1ms = 100us
*/
void Motor_PwmInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct = {0};
	TIM_OCInitTypeDef TIM_OCInitStruct = {0};
	//开启时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//GPIO配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                //推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  			 //选择管脚
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;        			 //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       			 //输出速度50MHz
	GPIO_Init(GPIOB, &GPIO_InitStruct);                   			 //初始化B组
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);			 //复用对象TIM3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);			 //复用对象TIM3
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;  		 //选择管脚
	GPIO_Init(GPIOD, &GPIO_InitStruct);                   			 //初始化B组
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);			 //复用对象TIM4
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);			 //复用对象TIM4
	//时基单元配置
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; 			 //时钟分割因子
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;   //向上计数模式
	TIM_BaseInitStruct.TIM_Prescaler = 84 - 1;                 //分频微秒级84
	TIM_BaseInitStruct.TIM_Period = 100;                       //定义PWM周期为10KHz
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStruct);                //初始化定时器3
	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStruct);                //初始化定时器4
	//OC初始化设置
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;             //PWM模式1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //输出状态使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;     //初始状态高电平
	TIM_OCInitStruct.TIM_Pulse = 50;                   //占空比设置
	//初始化对应通道
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);                       //初始化定时器3通道3
	TIM_OC4Init(TIM3, &TIM_OCInitStruct);                       //初始化定时器3通道4
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);           //使能TIM3通道3预装载寄存器
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);           //使能TIM3通道4预装载寄存器
	TIM_ARRPreloadConfig(TIM3, ENABLE);                         //使能TIM3预分频寄存器
	
	TIM_OC1Init(TIM4, &TIM_OCInitStruct);                       //初始化定时器4通道1
	TIM_OC2Init(TIM4, &TIM_OCInitStruct);                       //初始化定时器4通道2
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);           //使能TIM3通道3预装载寄存器
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);           //使能TIM3通道4预装载寄存器
	TIM_ARRPreloadConfig(TIM4, ENABLE);                         //使能TIM3预分频寄存器
	//使能定时器
	TIM_Cmd(TIM3, ENABLE);                                      //使能定时器3
	TIM_Cmd(TIM4, ENABLE);                                      //使能定时器4
}

/* 小车前进 */
void Motor_Forward(void)
{
	LF_FORWARD();
	LR_FORWARD();
	RF_FORWARD();
	RR_FORWARD();
}

/* 小车后退 */
void Motor_Backward(void)
{
	LF_BACKWARD();
	LR_BACKWARD();
	RF_BACKWARD();
	RR_BACKWARD();
}

/* 小车右移 */
void Motor_MoveRight(void)
{
	LF_FORWARD();		//左前前转
	LR_BACKWARD();	//左后停止
	RF_BACKWARD();	//右前停止
	RR_FORWARD();		//右后前转
}

/* 小车左移 */
void Motor_MoveLeft(void)
{
	LF_BACKWARD();	//左前停止
	LR_FORWARD();		//左后前转
	RF_FORWARD();		//右前前转
	RR_BACKWARD();	//右后停止
}

/* 小车右转 */
void Motor_TurnRight(void)
{
	LF_FORWARD();		//左前前转
	LR_FORWARD();		//左后前转
	RF_SHORTBRAKE();	//右前停止
	RR_SHORTBRAKE();	//右后停止
}

/* 小车左转 */
void Motor_TurnLeft(void)
{
	LF_SHORTBRAKE();	//左前停止
	LR_SHORTBRAKE();		//左后前转
	RF_FORWARD();		//右前前转
	RR_FORWARD();		//右后前转
}

/* 小车右旋 */
void Motor_SpinRight(void)
{
	LF_FORWARD();		//左前前转
	LR_FORWARD();		//左后后转
	RF_BACKWARD();		//右前后转
	RR_BACKWARD();		//右后前转
}

/* 小车左旋 */
void Motor_SpinLeft(void)
{
	LF_BACKWARD();		//左前后转
	LR_BACKWARD();		//左后前转
	RF_FORWARD();		//右前前转
	RR_FORWARD();		//右后后转
}

/* 小车停止 */
void Motor_Stop(void)
{
	LF_SHORTBRAKE();	//左前停止
	LR_SHORTBRAKE();	//左后停止
	RF_SHORTBRAKE();	//右前停止
	RR_SHORTBRAKE();	//右后停止
}

/* 小车后退一步 */
void Motor_StepBack(void)
{
	Motor_Stop();
	delay_ms(20);
	Motor_Backward();
	delay_ms(100);
	Motor_Stop();
}

/*
	* 函数名称   ： Motor_SetSpeed
	* 函数功能   ： 设置小车的速度
	* 输入参数   ： 取值范围1~4,speed=4小车速度最快,speed=1小车速度最慢
	* 函数返回值 ： 无
	* 函数描述   ： 无
*/
void Motor_SetSpeed(uint8_t speed)
{
	switch (speed) {
		case 1:
			TIM_SetCompare1(TIM4, 25);
			TIM_SetCompare2(TIM4, 25);
			TIM_SetCompare3(TIM3, 25);
			TIM_SetCompare4(TIM3, 25);
			break;
		case 2:
			TIM_SetCompare1(TIM4, 50);
			TIM_SetCompare2(TIM4, 50);
			TIM_SetCompare3(TIM3, 50);
			TIM_SetCompare4(TIM3, 50);
			break;
		case 3:
			TIM_SetCompare1(TIM4, 75);
			TIM_SetCompare2(TIM4, 75);
			TIM_SetCompare3(TIM3, 75);
			TIM_SetCompare4(TIM3, 75);
			break;
		case 4:
			TIM_SetCompare1(TIM4, 100);
			TIM_SetCompare2(TIM4, 100);
			TIM_SetCompare3(TIM3, 100);
			TIM_SetCompare4(TIM3, 100);
			break;
		default:
			printf("%s: arg is error\r\n", __FUNCTION__);
			break;
	}
}
