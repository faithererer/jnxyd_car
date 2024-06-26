#include "sr04.h"
#include "timer.h"

static void Sr04_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	/* ʹ�� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(SR04_TRIG_CLK|SR04_ECHO_CLK, ENABLE);

	/* ���� SR04_TRIG ����: ��ͨ������� */
	GPIO_InitStructure.GPIO_Pin = SR04_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(SR04_TRIG_PORT, &GPIO_InitStructure);
	
	/* ���� SR04_ECHO ����: ��ͨ�������� */
	GPIO_InitStructure.GPIO_Pin = SR04_ECHO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(SR04_ECHO_PORT, &GPIO_InitStructure);
}

static void Sr04_TIMInit(void)
{
	/* ��ʱ��TIM2��ʼ�� */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	/* ʹ�ܶ�ʱ��TIM2ʱ�� */
	SR04_TIM_CLK_INIT(SR04_TIM_CLK, ENABLE);
	TIM_DeInit(SR04_TIM);
	TIM_TimeBaseInitStruct.TIM_Period=SR04_TIM_ARR;				//��ʱ����Ϊ23200(0.58*40000)  
	TIM_TimeBaseInitStruct.TIM_Prescaler=SR04_TIM_PSC; 			//��Ƶϵ��Ϊ83,����������Ϊ1us
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;		//����Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInit(SR04_TIM, &TIM_TimeBaseInitStruct);
	TIM_Cmd(SR04_TIM, DISABLE);//�رն�ʱ��ʹ��  
	TIM_ClearFlag(SR04_TIM, TIM_FLAG_Update);	
}
	
void Sr04_Init(void)
{
	Sr04_GPIOInit();
	Sr04_TIMInit();
}

/*
	* ��������   �� Sr04_Measure
	* ��������   �� ��ȡ���Ծ���
	* �����β�   �� ��
	* ��������ֵ �� �ɹ����ز��Ծ���,�������Ծ��뷵��-1,��·�����쳣����-2
	* ��������   �� �����������Ϊ 60ms ����, �Է�ֹ�����źŶԻ����źŵ�Ӱ��
*/
float Sr04_Measure(void)
{
	float distance;
	uint32_t time = 20000;
	/* ���������ź� */
	SR04_TRIG_Low();
	SR04_TRIG_High();				
	delay_us(20);					
	SR04_TRIG_Low();
	/* �������źŸߵ�ƽ����ʱ�� */
	while ( !SRO4_ECHO_IN() ) 
	{		//�ȴ������źű�Ϊ�ߵ�ƽ
		if ( ! time-- )
			return -2;				//��·�����쳣
	}
	TIM_SetCounter(SR04_TIM, 0);		//��ն�ʱ��������
	TIM_Cmd(SR04_TIM, ENABLE);			//������ʱ��
	while ( SRO4_ECHO_IN() ) 
	{		//�ȴ������źű�Ϊ�͵�ƽ
		if ( TIM_GetFlagStatus(SR04_TIM, TIM_FLAG_Update) ) 
		{	
			TIM_ClearFlag(SR04_TIM, TIM_FLAG_Update);
			TIM_Cmd(SR04_TIM, DISABLE);	
			return -1;				//����������Χ(2cm~400cm)
		}
	}		
	TIM_Cmd(SR04_TIM, DISABLE);			//�رն�ʱ��
	time = TIM_GetCounter(SR04_TIM);	//��ȡ��ʱ��������ֵ,�������źŸߵ�ƽʱ��us
	/* ������Ծ��� */
	distance = time / 58.0;
	return distance;
}
