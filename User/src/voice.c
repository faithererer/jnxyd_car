#include "voice.h"
#include <stdio.h>

/*************���ŷֲ�**************
	RX--USART3_TX--PB10
	TX--USART3_RX--PB11
***********************************/

static uint8_t sndBuf[10];	//��¼�����ֽ�����
static uint8_t rcvBuf[10];	//��¼�����ֽ�����
static uint8_t rcvCount;	//��¼�����ֽڸ���
static uint8_t voiceAck;	//һ֡���ݽ�����ɱ�־λ


void Voice_Init(void)
{
	Voice_GPIOInit();
	Voice_UARTInit();
}

void Voice_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* ��GPIOB��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	/* PB10--����������� */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	/* PB11--���ø������� */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);	
}

void Voice_UARTInit(void)
{
	USART_InitTypeDef USART_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	/* �򿪴���3ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	/* ����3��ʼ�� */
	USART_InitStruct.USART_BaudRate = 9600;//������
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//���ͽ���
	USART_InitStruct.USART_Parity = USART_Parity_No;//��żʧ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//��֡��β���� 1 ��ֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8 λ����
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ��������ʧ��
	USART_Init(USART3, &USART_InitStruct);	
	/* ��ʼ������3�ж� */
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	/* ʹ�ܴ���3�����жϺͿ����ж� */
	USART_ITConfig(USART3, USART_IT_RXNE|USART_IT_IDLE, ENABLE);
	/* ʹ�ܴ���3 */
	USART_Cmd(USART3, ENABLE);
}

/* ���У�� */
static void DoSum(unsigned char *Str, unsigned int len)
{
	u8 i;
	u16 xorsum = 0;
	for (i=0;i<len;i++) 
	{
		xorsum = xorsum + Str[i];
	}
	xorsum = 0 - xorsum;
	*(Str+i) = (u8)(xorsum >>8);//�õ����ֽ�
	*(Str+i+1) = (u8)(xorsum & 0x00ff);//�õ����ֽ�
}

/******************************************************************************
- ���������� �������ⷢ������[�������ƺͲ�ѯ]
- ����˵���� cmd:��ʾ����ָ������ָ�����������ѯ�����ָ��
			 feedback:�Ƿ���ҪӦ��[0:����ҪӦ��1:��ҪӦ��]
			 data:���͵Ĳ���
******************************************************************************/
void VoiceSendCMD(uint8_t cmd ,uint8_t feedback ,uint16_t dat)
{
	uint8_t i = 0;
	sndBuf[0] = 0x7E; 			//��ʼλ
	sndBuf[1] = 0xFF; 			//�����ֽ�
	sndBuf[2] = 0x06; 			//����
	sndBuf[3] = cmd; 			//����ָ��
	sndBuf[4] = feedback;		//�Ƿ���Ҫ����
	sndBuf[5] = (u8)(dat >> 8);//datah
	sndBuf[6] = (u8)(dat); 		//datal
	DoSum(&sndBuf[1],6); 		//��У���
	sndBuf[9] = 0xEF; 			//����λ
	/* ���ʹ�֡���� */
	for (i=0;i<10;i++) 
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); // �ȴ�ǰһ�ַ�������
		USART_SendData(USART3, sndBuf[i]);
	}
}

/******************************************************************************
- ���������� ����ģ��У��Ӧ��
- ����˵���� CMD:��ʾ����ָ������ָ�����������ѯ�����ָ��
			 feedback:�Ƿ���ҪӦ��[0:����ҪӦ��1:��ҪӦ��]
			 data:���͵Ĳ���
-   ����ֵ�� У��ɹ�����0��ʧ�ܷ���-1	
******************************************************************************/
int VoiceCheckACK(uint8_t cmd ,uint8_t feedback ,uint16_t dat)
{
	u8 i;
	u16 xorsum = 0;
	u16 xorsum1 = 0;
	
	while (!voiceAck) ;	//�ȴ�һ֡���ݽ������
	voiceAck = 0;		//�������ɱ�־λ
	
	if ( rcvBuf[0]!=0x7E && rcvBuf[1]!=0xFF && rcvBuf[2]!=0x06 && rcvBuf[9]!=0xEF) 
	{
		printf("��������֡��ʽ����\n");
		return -1;
	}
	
	for (i=1;i<7;i++) 
	{	//�����ܵ������ݽ����ۼ�
		xorsum = xorsum + rcvBuf[i] ;
	}
	xorsum1 = ((u16)(rcvBuf[i]<<8)) | (rcvBuf[i+1]);//�����ǽ��յ���У���ֽ�,16λ
	xorsum = xorsum + xorsum1;			//�����յ���У���ֽں��Լ����У���������
	
	if (xorsum) 
	{
		printf("У��ʧ��\n");
		return -1;
	}
//	if (!xorsum) {			//���ڴ���--�Խ��յ���ָ����д���
//		printf("У��ɹ�\n");
//	} else {				//У�������֮��Ĵ���
//		printf("У��ʧ��\n");
//	}		
	return 0;
}


void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE)) 
	{	
		rcvBuf[rcvCount++] = USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
	if (USART_GetITStatus(USART3, USART_IT_IDLE)) 
	{
		rcvCount = 0;
		voiceAck = 1;
		USART3->SR;
		USART3->DR;
	}
}


