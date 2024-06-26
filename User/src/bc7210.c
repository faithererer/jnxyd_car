#include "bc7210.h"
#include "motor.h"

infrared_t ir_data;

static void Bc7210_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB,ENABLE);
	/* IR_SS--���ø�������PA15 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(IR_SS_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SPI3);
	/* IR_DAT--���ø�������PB5 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);
	/* IR_SCK--���ø�������PB3 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);
	/* IR_ACK--��ͨ�������� */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);		
}

static void Bc7210_SPIInit(void)
{
	SPI_InitTypeDef SPI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	/* ��SPIʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	/* ��ʼ��SPI */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI ����Ϊ˫��˫��ȫ˫��
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;     //����Ϊ��SPI 
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//SPI ���ͽ��� 8 λ֡�ṹ
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;       //ʱ�����յ�
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;     //���ݲ����ڵ�һ��ʱ����
	SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;        //�ڲ� NSS �ź��� SSI λ����
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_LSB;//���ݴ���� LSB λ��ʼ
	SPI_InitStruct.SPI_CRCPolynomial = 7;          //���� CRC ֵ����Ķ���ʽ
	SPI_Init(SPI3, &SPI_InitStruct);
	/* ʹ��SPI */
	SPI_Cmd(SPI3, ENABLE);
	/* ʹ��SPI�����ж� */
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);
	/* ��ʼ��SPI�ж� */
	NVIC_InitStruct.NVIC_IRQChannel = SPI3_IRQn;//SPI3 ȫ���ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//�����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void Bc7210_Init(void)
{
	Bc7210_GPIOInit();
	Bc7210_SPIInit();
}

/* SPI1�жϷ����� */
void IR_SPI_IRQHandler(void)
{
	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE)) 
	{
		/* ��ȡ���ݱ����ձ�־ */
		ir_data.ir_buff[ir_data.ir_len++] = SPI_I2S_ReceiveData(SPI3);
		if (ir_data.ir_len >= 3) 
		{
			ir_data.ir_len = 0;
			ir_data.ir_flag = 1;
		}
	}
}

/*
	* ��������   �� IR_DataKey
	* ��������   �� ��ȡ����ң�����İ���ֵ
	* �����β�   �� ��
	* ��������ֵ �� �ް������·���0,�а������·��ذ���ֵ
	* ��������   �� ��
*/
uint8_t IR_DataKey(void)
{
	static uint8_t key_value = 0;	//����ң��������ֵ
	static uint8_t key_flag = 0;	//����ң�����������±�־
	switch (key_flag) 
	{
		case 0://����������ʱ
			if ( !IR_ACT_IN() && ir_data.ir_flag ) 
			{	
				//����������,IR_ACT��Ϊ�͵�ƽ,�������������ɱ�־��1
				if ( ir_data.ir_buff[0]+ir_data.ir_buff[1]==0xff ) 
				{	
					//����У��					
					key_value = ir_data.ir_buff[2];
				}	
				ir_data.ir_flag = 0;//��պ������������ɱ�־
				key_flag = 1;
			}
			break;
		case 1://����������ʱ
			if ( IR_ACT_IN() || key_value==0x44 || key_value==0x40) 
			{ 
				//�������ɿ�,IR_ACTΪ�ߵ�ƽ
				key_flag = 0;
				key_value = 0;
			}
			break;
	}
	return key_value;
}

/*
����ң��������--��ӦС������
	<<			ѡ����һ������ģʽ
	>>			ѡ����һ������ģʽ
	+			��̨����
	100+		��̨����
	0			��̨����
	200+		��̨����
	2			С��ǰ��
	8			С������
	4			С������
	6			С������
	7			С������
	9			С������
	1			С����ת
	3			С����ת
*/
void InfraredControl(void)
{
	uint8_t key = IR_DataKey();
	
	switch (key) 
	{
		case 0x18: 	Motor_Forward(); 	break;
//		case 0x5a:  // 6��ת
		
	}
}

