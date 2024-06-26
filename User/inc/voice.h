#ifndef __VOICE_H_
#define __VOICE_H_

#include "stm32f4xx.h"

/******************控制命令*****************
	0x01 下一曲
	0x02 上一曲
	0x03 指定曲目(根目录) 
	0x04 音量+
	0x05 音量-
	0x06 指定音量 
	0x07 保留 
	0x08 单曲循环指定曲目播放（根目录）
	0x09 指定播放设备 
	0x0A 进入睡眠 -- 低功耗 
	0x0B 唤醒睡眠
	0x0C 芯片复位
	0x0D 播放
	0x0E 暂停
	0x0F 指定文件夹文件名播放 
	0x13 插播广告 
	0x14 单个文件夹支持1000首曲目 
	0x15 停止插播播放背景音乐
	0x16 停止
	0x17 指定文件夹循环播放 
	0x18 指定整个设备随机播放 
	0x19 对当前播放的曲目设置为循环播放 
	0x1A 开启和关闭芯片的 DAC 输出 
	0x21 保留 
	0x25 多文件夹插播 
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

