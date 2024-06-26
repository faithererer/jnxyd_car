#include "stm32f4xx.h"
#include "motor.h"
#include "timer.h"
#include <stdio.h>

/**************ֱ��������ŷֲ�************
	��� L_AIN1--PD8 		L_AIN2--PD9 		L_PWMA--PWM1--PD12--TIM4_CH1
	��ǰ L_BIN1--PD10		L_BIN2--PD11		L_PWMB--PWM2--PD13--TIM4_CH2
	��ǰ R_AIN1--PE15		R_AIN2--PE14		R_PWMA--PWM3--PB0---TIM3_CH3
	�Һ� R_BIN1--PE12		R_BIN2--PE11		R_PWMB--PWM4--PB1---TIM3_CH4

	�����ʹ�� L_STBY--PD14
	�ұ���ʹ�� R_STBY--PE13
******************************************/
 
void Motor_Init(void)
{
	Motor_GPIOInit();
	Motor_PwmInit();
	//STBYʹ��	
	LSTBY_STATUS(ENABLE);
	RSTBY_STATUS(ENABLE);
}
 
 
void Motor_GPIOInit(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//����ʱ�ӣ�GPIOB   GPIOD  GPIOE��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	//ģʽ����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;   //���ģʽ  
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;  //�������������ߵ͵�ƽ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//����ٶ�
	//���motor1 ��ǰmotor2
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	//��ǰmotor3 �Һ�motor4
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStruct);                
	//ʹ�� -- STBY
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_14 ;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 ;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}
/*
	* ��������   �� MOTOR_PwmInit
	* ��������   �� ���PWM��ʼ��
	* �����β�   �� ��
	* ��������ֵ �� ��
	* ��������   �� ��
L_PWMA--PWM1--PD12--TIM4_CH1
L_PWMB--PWM2--PD13--TIM4_CH2
R_PWMA--PWM3--PB0---TIM3_CH3
R_PWMB--PWM4--PB1---TIM3_CH4

��ʱ��Ƶ�ʣ�84 000 000Hz   1����84 000 000 ������
�����Ҫ10KHz��Ƶ�ʣ� �Ǿ������� = 1 / 10 000 = 0.1ms = 100us

��Ƶ      ��84           84 000 000 / 84 = 1 000 000
����ֵ    ��100  
           
84 * 100 = 84 00 
8400 / 84 000 000 = 1 / 10 000 = 0.1ms = 100us
*/
void Motor_PwmInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct = {0};
	TIM_OCInitTypeDef TIM_OCInitStruct = {0};
	//����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//GPIO����
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                  //���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                //�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  			 //ѡ��ܽ�
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;        			 //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       			 //����ٶ�50MHz
	GPIO_Init(GPIOB, &GPIO_InitStruct);                   			 //��ʼ��B��
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);			 //���ö���TIM3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);			 //���ö���TIM3
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;  		 //ѡ��ܽ�
	GPIO_Init(GPIOD, &GPIO_InitStruct);                   			 //��ʼ��B��
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);			 //���ö���TIM4
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);			 //���ö���TIM4
	//ʱ����Ԫ����
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; 			 //ʱ�ӷָ�����
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;   //���ϼ���ģʽ
	TIM_BaseInitStruct.TIM_Prescaler = 84 - 1;                 //��Ƶ΢�뼶84
	TIM_BaseInitStruct.TIM_Period = 100;                       //����PWM����Ϊ10KHz
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStruct);                //��ʼ����ʱ��3
	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStruct);                //��ʼ����ʱ��4
	//OC��ʼ������
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;             //PWMģʽ1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //���״̬ʹ��
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;     //��ʼ״̬�ߵ�ƽ
	TIM_OCInitStruct.TIM_Pulse = 50;                   //ռ�ձ�����
	//��ʼ����Ӧͨ��
	TIM_OC3Init(TIM3, &TIM_OCInitStruct);                       //��ʼ����ʱ��3ͨ��3
	TIM_OC4Init(TIM3, &TIM_OCInitStruct);                       //��ʼ����ʱ��3ͨ��4
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);           //ʹ��TIM3ͨ��3Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);           //ʹ��TIM3ͨ��4Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3, ENABLE);                         //ʹ��TIM3Ԥ��Ƶ�Ĵ���
	
	TIM_OC1Init(TIM4, &TIM_OCInitStruct);                       //��ʼ����ʱ��4ͨ��1
	TIM_OC2Init(TIM4, &TIM_OCInitStruct);                       //��ʼ����ʱ��4ͨ��2
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);           //ʹ��TIM3ͨ��3Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);           //ʹ��TIM3ͨ��4Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM4, ENABLE);                         //ʹ��TIM3Ԥ��Ƶ�Ĵ���
	//ʹ�ܶ�ʱ��
	TIM_Cmd(TIM3, ENABLE);                                      //ʹ�ܶ�ʱ��3
	TIM_Cmd(TIM4, ENABLE);                                      //ʹ�ܶ�ʱ��4
}

/* С��ǰ�� */
void Motor_Forward(void)
{
	LF_FORWARD();
	LR_FORWARD();
	RF_FORWARD();
	RR_FORWARD();
}

/* С������ */
void Motor_Backward(void)
{
	LF_BACKWARD();
	LR_BACKWARD();
	RF_BACKWARD();
	RR_BACKWARD();
}

/* С������ */
void Motor_MoveRight(void)
{
	LF_FORWARD();		//��ǰǰת
	LR_BACKWARD();	//���ֹͣ
	RF_BACKWARD();	//��ǰֹͣ
	RR_FORWARD();		//�Һ�ǰת
}

/* С������ */
void Motor_MoveLeft(void)
{
	LF_BACKWARD();	//��ǰֹͣ
	LR_FORWARD();		//���ǰת
	RF_FORWARD();		//��ǰǰת
	RR_BACKWARD();	//�Һ�ֹͣ
}

/* С����ת */
void Motor_TurnRight(void)
{
	LF_FORWARD();		//��ǰǰת
	LR_FORWARD();		//���ǰת
	RF_SHORTBRAKE();	//��ǰֹͣ
	RR_SHORTBRAKE();	//�Һ�ֹͣ
}

/* С����ת */
void Motor_TurnLeft(void)
{
	LF_SHORTBRAKE();	//��ǰֹͣ
	LR_SHORTBRAKE();		//���ǰת
	RF_FORWARD();		//��ǰǰת
	RR_FORWARD();		//�Һ�ǰת
}

/* С������ */
void Motor_SpinRight(void)
{
	LF_FORWARD();		//��ǰǰת
	LR_FORWARD();		//����ת
	RF_BACKWARD();		//��ǰ��ת
	RR_BACKWARD();		//�Һ�ǰת
}

/* С������ */
void Motor_SpinLeft(void)
{
	LF_BACKWARD();		//��ǰ��ת
	LR_BACKWARD();		//���ǰת
	RF_FORWARD();		//��ǰǰת
	RR_FORWARD();		//�Һ��ת
}

/* С��ֹͣ */
void Motor_Stop(void)
{
	LF_SHORTBRAKE();	//��ǰֹͣ
	LR_SHORTBRAKE();	//���ֹͣ
	RF_SHORTBRAKE();	//��ǰֹͣ
	RR_SHORTBRAKE();	//�Һ�ֹͣ
}

/* С������һ�� */
void Motor_StepBack(void)
{
	Motor_Stop();
	delay_ms(20);
	Motor_Backward();
	delay_ms(100);
	Motor_Stop();
}

/*
	* ��������   �� Motor_SetSpeed
	* ��������   �� ����С�����ٶ�
	* �������   �� ȡֵ��Χ1~4,speed=4С���ٶ����,speed=1С���ٶ�����
	* ��������ֵ �� ��
	* ��������   �� ��
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
