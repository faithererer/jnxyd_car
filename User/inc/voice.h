#ifndef __VOICE_H_
#define __VOICE_H_

#include "stm32f4xx.h"

/******************��������*****************
	0x01 ��һ��
	0x02 ��һ��
	0x03 ָ����Ŀ(��Ŀ¼) 
	0x04 ����+
	0x05 ����-
	0x06 ָ������ 
	0x07 ���� 
	0x08 ����ѭ��ָ����Ŀ���ţ���Ŀ¼��
	0x09 ָ�������豸 
	0x0A ����˯�� -- �͹��� 
	0x0B ����˯��
	0x0C оƬ��λ
	0x0D ����
	0x0E ��ͣ
	0x0F ָ���ļ����ļ������� 
	0x13 �岥��� 
	0x14 �����ļ���֧��1000����Ŀ 
	0x15 ֹͣ�岥���ű�������
	0x16 ֹͣ
	0x17 ָ���ļ���ѭ������ 
	0x18 ָ�������豸������� 
	0x19 �Ե�ǰ���ŵ���Ŀ����Ϊѭ������ 
	0x1A �����͹ر�оƬ�� DAC ��� 
	0x21 ���� 
	0x25 ���ļ��в岥 
*******************************************/

#define Voice_PlayNext()	do { VoiceSendCMD(0x01,0x00,0x00); } while(0)
#define Voice_PlayTest()	do { VoiceSendCMD(0x03,0x00,0x01); } while(0)
#define V_Down()	do { VoiceSendCMD(0x05,0x00,0x01); } while(0)

#define Voice_Play(x) do { VoiceSendCMD(0x03,0x00,x); } while(0)
void Voice_Init(void);
void Voice_GPIOInit(void);
void Voice_UARTInit(void);
void VoiceSendCMD(uint8_t cmd ,uint8_t feedback ,uint16_t dat);
int VoiceCheckACK(uint8_t cmd ,uint8_t feedback ,uint16_t dat);


#endif

