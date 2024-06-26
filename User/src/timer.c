#include "timer.h"

/*
*ע�⣺ TIM_TimeBaseInitTypeDef �ṹ�������� 5 ����Ա�� TIM6 �� TIM7 �ļĴ�������ֻ��
*TIM_Prescaler �� TIM_Period������ʹ�� TIM6 �� TIM7 ��ʱ��ֻ���ʼ����������Ա���ɣ�
* ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
*-----------------------------------------------------------------------
* TIM_Prescaler ����
* TIM_CounterMode TIMx,x[6,7]û�У��������У�������ʱ����
* TIM_Period ����
* TIM_ClockDivision TIMx,x[6,7]û�У���������(������ʱ��)
* TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
*-----------------------------------------------------------------------
*/


/*
* ��������  �� TIM6_Delay_Init
* ��������  �� ��ʱ��6��ʼ��
* �����β�  �� ��
* ��������ֵ�� ��
* ��������  �� APB1 -- 84MHz
*/
void TIM6_Delay_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);         //ʹ�ܶ�ʱ��ʱ��
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;    //ʱ�ӷָ�
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);             //��ʱ����ʼ��
	
	TIM_ARRPreloadConfig(TIM6,ENABLE);                          //ʹ���Զ���װ�ؼĴ���
	TIM_SelectOnePulseMode(TIM6,TIM_OPMode_Repetitive);         //ѭ������
	TIM_UpdateRequestConfig(TIM6,TIM_UpdateSource_Global);
	TIM_UpdateDisableConfig(TIM6,DISABLE);                      //ʹ�ܸ����¼�
	
	TIM_Cmd(TIM6,DISABLE);
}
/*
* ��������  �� Tim6_Delay_ms
* ��������  �� ��ʱ��6���뼶��ʱ����
* �����β�  �� u16 ms
* ��������ֵ�� ��
* ��������  �� APB1 -- 84MHz
*/
void Tim6_Delay_ms(u16 ms)
{
	TIM_PrescalerConfig(TIM6,8400-1,TIM_PSCReloadMode_Update);   //����Ԥ��Ƶϵ��
	TIM_SetAutoreload(TIM6,10*ms);                                //��װ��ֵ
	
	TIM_GenerateEvent(TIM6,TIM_EventSource_Update);               //������������¼�
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);                          //�����־λ����
	TIM_Cmd(TIM6,ENABLE);               
	
	while(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == RESET)
	{
		/* �ȴ�ʱ�䵽�� */
	}
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,DISABLE);
}
/*
* ��������  �� Tim6_Delay_us
* ��������  �� ��ʱ��6΢�뼶��ʱ����
* �����β�  �� u16 us
* ��������ֵ�� ��
* ��������  �� APB1 -- 84MHz
*/
void Tim6_Delay_us(u16 us)
{
	TIM_PrescalerConfig(TIM6,84-1,TIM_PSCReloadMode_Update);   //����Ԥ��Ƶϵ��
	TIM_SetAutoreload(TIM6,us);                                //��װ��ֵ
	
	TIM_GenerateEvent(TIM6,TIM_EventSource_Update);               //������������¼�
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);                          //�����־λ����
	TIM_Cmd(TIM6,ENABLE);               
	
	while(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == RESET)
	{
		/* �ȴ�ʱ�䵽�� */
	}
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,DISABLE);
	
}


