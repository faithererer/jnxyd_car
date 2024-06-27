#include "voice.h"
#include <stdio.h>

/*************引脚分布**************
	RX--USART3_TX--PB10
	TX--USART3_RX--PB11
***********************************/

static uint8_t sndBuf[10];	//记录发送字节数组
static uint8_t rcvBuf[10];	//记录接收字节数组
static uint8_t rcvCount;	//记录接收字节个数
static uint8_t voiceAck;	//一帧数据接收完成标志位


void Voice_Init(void)
{
	Voice_GPIOInit();
	Voice_UARTInit();
}

void Voice_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* 打开GPIOB的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	/* PB10--复用推挽输出 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	/* PB11--复用浮空输入 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);	
}

void Voice_UARTInit(void)
{
	USART_InitTypeDef USART_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	/* 打开串口3时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	/* 串口3初始化 */
	USART_InitStruct.USART_BaudRate = 9600;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//发送接收
	USART_InitStruct.USART_Parity = USART_Parity_No;//奇偶失能
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//在帧结尾传输 1 个停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//8 位数据
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制失能
	USART_Init(USART3, &USART_InitStruct);	
	/* 初始化串口3中断 */
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	/* 使能串口3接收中断和空闲中断 */
	USART_ITConfig(USART3, USART_IT_RXNE|USART_IT_IDLE, ENABLE);
	/* 使能串口3 */
	USART_Cmd(USART3, ENABLE);
}

/* 求和校验 */
static void DoSum(unsigned char *Str, unsigned int len)
{
	u8 i;
	u16 xorsum = 0;
	for (i=0;i<len;i++) 
	{
		xorsum = xorsum + Str[i];
	}
	xorsum = 0 - xorsum;
	*(Str+i) = (u8)(xorsum >>8);//得到高字节
	*(Str+i+1) = (u8)(xorsum & 0x00ff);//得到低字节
}

/******************************************************************************
- 功能描述： 串口向外发送命令[包括控制和查询]
- 参数说明： cmd:表示控制指令，请查阅指令表，还包括查询的相关指令
			 feedback:是否需要应答[0:不需要应答，1:需要应答]
			 data:传送的参数
******************************************************************************/
void VoiceSendCMD(uint8_t cmd ,uint8_t feedback ,uint16_t dat)
{
	uint8_t i = 0;
	sndBuf[0] = 0x7E; 			//起始位
	sndBuf[1] = 0xFF; 			//保留字节
	sndBuf[2] = 0x06; 			//长度
	sndBuf[3] = cmd; 			//控制指令
	sndBuf[4] = feedback;		//是否需要反馈
	sndBuf[5] = (u8)(dat >> 8);//datah
	sndBuf[6] = (u8)(dat); 		//datal
	DoSum(&sndBuf[1],6); 		//求校验和
	sndBuf[9] = 0xEF; 			//结束位
	/* 发送此帧数据 */
	for (i=0;i<10;i++) 
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); // 等待前一字符发送完
		USART_SendData(USART3, sndBuf[i]);
	}
}

/******************************************************************************
- 功能描述： 语音模块校验应答
- 参数说明： CMD:表示控制指令，请查阅指令表，还包括查询的相关指令
			 feedback:是否需要应答[0:不需要应答，1:需要应答]
			 data:传送的参数
-   返回值： 校验成功返回0，失败返回-1	
******************************************************************************/
int VoiceCheckACK(uint8_t cmd ,uint8_t feedback ,uint16_t dat)
{
	u8 i;
	u16 xorsum = 0;
	u16 xorsum1 = 0;
	
	while (!voiceAck) ;	//等待一帧数据接收完毕
	voiceAck = 0;		//清接收完成标志位
	
	if ( rcvBuf[0]!=0x7E && rcvBuf[1]!=0xFF && rcvBuf[2]!=0x06 && rcvBuf[9]!=0xEF) 
	{
		printf("返回数据帧格式错误\n");
		return -1;
	}
	
	for (i=1;i<7;i++) 
	{	//将接受到的数据进行累加
		xorsum = xorsum + rcvBuf[i] ;
	}
	xorsum1 = ((u16)(rcvBuf[i]<<8)) | (rcvBuf[i+1]);//这里是接收到的校验字节,16位
	xorsum = xorsum + xorsum1;			//将接收到的校验字节和自己算的校验数据相加
	
	if (xorsum) 
	{
		printf("校验失败\n");
		return -1;
	}
//	if (!xorsum) {			//串口处理--对接收到的指令进行处理
//		printf("校验成功\n");
//	} else {				//校验码出错之后的处理
//		printf("校验失败\n");
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


