#include "usart.h"
#include <stdio.h>
#include "stm32f4xx.h"

/* 初始化串口 */
void usart_init(int baud_rate)
{
	GPIO_InitTypeDef GPIO = {0};
	USART_InitTypeDef USART = {0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	GPIO.GPIO_Mode = GPIO_Mode_AF;
	GPIO.GPIO_OType = GPIO_OType_PP;
	GPIO.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA, &GPIO);

	USART_StructInit(&USART); 
	USART.USART_BaudRate = baud_rate;
	USART_Init(USART1,&USART);
	USART_Cmd(USART1, ENABLE);
}

/* 串口发送一个字符 */
static void usart_sendbyte(char ch)
{
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	// 等待前一字符发送完毕
  USART_SendData(USART1, ch);
}

// printf会调用此函数
int fputc(int ch, FILE *fp)
{
	usart_sendbyte(ch);
	return ch;
}
