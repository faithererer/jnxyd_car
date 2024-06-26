#include "bc7210.h"
#include "motor.h"

infrared_t ir_data;

static void Bc7210_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB,ENABLE);
	/* IR_SS--复用浮空输入PA15 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(IR_SS_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SPI3);
	/* IR_DAT--复用浮空输入PB5 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);
	/* IR_SCK--复用浮空输入PB3 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);
	/* IR_ACK--普通浮空输入 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);		
}

static void Bc7210_SPIInit(void)
{
	SPI_InitTypeDef SPI_InitStruct = {0};
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	/* 打开SPI时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	/* 初始化SPI */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI 设置为双线双向全双工
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;     //设置为从SPI 
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//SPI 发送接收 8 位帧结构
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;       //时钟悬空低
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;     //数据捕获于第一个时钟沿
	SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;        //内部 NSS 信号有 SSI 位控制
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_LSB;//数据传输从 LSB 位开始
	SPI_InitStruct.SPI_CRCPolynomial = 7;          //用于 CRC 值计算的多项式
	SPI_Init(SPI3, &SPI_InitStruct);
	/* 使能SPI */
	SPI_Cmd(SPI3, ENABLE);
	/* 使能SPI接收中断 */
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);
	/* 初始化SPI中断 */
	NVIC_InitStruct.NVIC_IRQChannel = SPI3_IRQn;//SPI3 全局中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//先占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//从优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void Bc7210_Init(void)
{
	Bc7210_GPIOInit();
	Bc7210_SPIInit();
}

/* SPI1中断服务函数 */
void IR_SPI_IRQHandler(void)
{
	if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE)) 
	{
		/* 读取数据便会清空标志 */
		ir_data.ir_buff[ir_data.ir_len++] = SPI_I2S_ReceiveData(SPI3);
		if (ir_data.ir_len >= 3) 
		{
			ir_data.ir_len = 0;
			ir_data.ir_flag = 1;
		}
	}
}

/*
	* 函数名称   ： IR_DataKey
	* 函数功能   ： 获取红外遥控器的按键值
	* 函数形参   ： 无
	* 函数返回值 ： 无按键按下返回0,有按键按下返回按键值
	* 函数描述   ： 无
*/
uint8_t IR_DataKey(void)
{
	static uint8_t key_value = 0;	//红外遥控器按键值
	static uint8_t key_flag = 0;	//红外遥控器按键按下标志
	switch (key_flag) 
	{
		case 0://当按键按下时
			if ( !IR_ACT_IN() && ir_data.ir_flag ) 
			{	
				//当按键按下,IR_ACT变为低电平,红外接收数据完成标志置1
				if ( ir_data.ir_buff[0]+ir_data.ir_buff[1]==0xff ) 
				{	
					//进行校验					
					key_value = ir_data.ir_buff[2];
				}	
				ir_data.ir_flag = 0;//清空红外接收数据完成标志
				key_flag = 1;
			}
			break;
		case 1://当按键松手时
			if ( IR_ACT_IN() || key_value==0x44 || key_value==0x40) 
			{ 
				//当按键松开,IR_ACT为高电平
				key_flag = 0;
				key_value = 0;
			}
			break;
	}
	return key_value;
}

/*
红外遥控器按键--对应小车动作
	<<			选择上一个工作模式
	>>			选择下一个工作模式
	+			云台向上
	100+		云台向下
	0			云台向左
	200+		云台向右
	2			小车前进
	8			小车后退
	4			小车左移
	6			小车右移
	7			小车左旋
	9			小车右旋
	1			小车左转
	3			小车右转
*/
void InfraredControl(void)
{
	uint8_t key = IR_DataKey();
	
	switch (key) 
	{
		case 0x18: 	Motor_Forward(); 	break;
//		case 0x5a:  // 6右转
		
	}
}

