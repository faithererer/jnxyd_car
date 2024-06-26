#include "oled.h"
#include "DZK.h"
#include "font.h"
#include "timer.h"
#include <string.h>

static void OLED_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	/*!< 使能 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd (OLED_CS_CLK|OLED_DC_CLK|OLED_RST_CLK|OLED_SCLK_CLK|OLED_MOSI_CLK, ENABLE);

	/*!< 配置 OLED_CS 引脚: 普通推挽输出 */
	GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(OLED_CS_PORT, &GPIO_InitStructure);
	
	/*!< 配置 OLED_DC 引脚: 普通推挽输出 */
	GPIO_InitStructure.GPIO_Pin = OLED_DC_PIN;
	GPIO_Init(OLED_DC_PORT, &GPIO_InitStructure);
	
	/*!< 配置 OLED_RST 引脚: 普通推挽输出 */
	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
	GPIO_Init(OLED_RST_PORT, &GPIO_InitStructure);
	
	/*!< 配置 OLED_SCLK 引脚: 复用为SPI1 */
	GPIO_InitStructure.GPIO_Pin = OLED_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(OLED_SCLK_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(OLED_SCLK_PORT, OLED_SCLK_PINSOURCE, OLED_SCLK_AF);

	/*!< 配置 OLED_MOSI 引脚: 复用为SPI1 */
	GPIO_InitStructure.GPIO_Pin = OLED_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(OLED_MOSI_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(OLED_MOSI_PORT, OLED_MOSI_PINSOURCE, OLED_MOSI_AF);
}

static void OLED_SPIInit(void)
{
	SPI_InitTypeDef SPI_InitStruct = {0};
	/*!< 使能SPI时钟 */
	OLED_SPI_CLK_INIT(OLED_SPI_CLK,ENABLE);
	/*!< SPI初始化  */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = SPI_CRC_Rx | SPI_CRC_Tx;
	SPI_Init(OLED_SPI, &SPI_InitStruct);
	/* 使能SPI */
	SPI_Cmd(OLED_SPI, ENABLE);
}

/************************************************
功能：SPI发送字节函数
************************************************/
static char SPI_WiteByte(uint8_t data)
{
	uint8_t retry = 0;
	while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET) 
	{	//等待发送区空
		retry++;
		if (retry > 200)	
			return -1;
	}
	SPI_I2S_SendData(OLED_SPI, data); //通过外设 SPI1 发送一个数据
	return 0;
//	while( ! SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));  //等待数据发送完成
//	SPI_I2S_SendData(SPI1,data);
//	while(! SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));  //等待接收完成
//	return SPI_I2S_ReceiveData(SPI1);
}

#define OLED_WriteByte(data)	SPI_WiteByte(data)

/* OLED写命令 */
static void OLED_WR_Com(uint8_t Com)
{
	OLED_CS_Low();
	OLED_DC_Low();
	OLED_WriteByte(Com);
	OLED_CS_High();
}

/* OLED写数据 */
static void OLED_WR_Dat(uint8_t Dat)
{
	OLED_CS_Low();
	OLED_DC_High();
	OLED_WriteByte(Dat);
	OLED_CS_High();
}

/*
	作用：打点
	参数：page--第几页   colum--第几页
	返回值：无
*/
static void OLED_Setpos(int page,int colum)
{
   OLED_WR_Com(0xB0+page);
   OLED_WR_Com(((colum&0xf0)>>4)|0x10);//&操作优先级比较低 需要加括号
   OLED_WR_Com((colum&0x0f));
}

/* OLED清屏函数 */
void OLED_Clear(void)
{
	int x , y;

	for(y = 0;y <= 7;y++)
	{
		OLED_Setpos(y,0);
		for(x = 0;x <= 128;x++)
		{
			OLED_WR_Dat(0x00);    //写入数据0，清屏	    	
		}
	}	
}

/* OLED初始化 */
void OLED_Init(void)
{
	OLED_GPIOInit();
	OLED_SPIInit();
	
	OLED_RST_Low();
	delay_ms(100);
	OLED_RST_High();

	OLED_WR_Com(0xAE);//--turn off oled panel
	OLED_WR_Com(0x00);//---set low column address
	OLED_WR_Com(0x10);//---set high column address
	OLED_WR_Com(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Com(0x81);//--set contrast control register
	OLED_WR_Com(0xCF); // Set SEG Output Current Brightness
	OLED_WR_Com(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Com(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Com(0xA6);//--set normal display
	OLED_WR_Com(0xA8);//--set multiplex ratio(1 to 64)
	OLED_WR_Com(0x3f);//--1/64 duty
	OLED_WR_Com(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Com(0x00);//-not offset
	OLED_WR_Com(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Com(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Com(0xD9);//--set pre-charge period
	OLED_WR_Com(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Com(0xDA);//--set com pins hardware configuration
	OLED_WR_Com(0x12);
	OLED_WR_Com(0xDB);//--set vcomh
	OLED_WR_Com(0x40);//Set VCOM Deselect Level
	OLED_WR_Com(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Com(0x02);//
	OLED_WR_Com(0x8D);//--set Charge Pump enable/disable
	OLED_WR_Com(0x14);//--set(0x10) disable
	OLED_WR_Com(0xA4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Com(0xA6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Com(0xAF);//--turn on oled panel
			
	OLED_WR_Com(0xAF); /*display ON*/ 

	OLED_Clear();
}


/*
	显示Ascii码字符
	page---页地址（0-7）
	colum--列地址（0-128）
*/
// add
typedef uint8_t u8;
typedef uint16_t u16;
static void OledDisplayAscii(char page,char colum,char ch)
{
	u8 buff[16],i,j;
	ch = ch - 32;
	memcpy(buff,&Ascii[ch*16],16);
	for(i = 0 ; i < 2; i++)
	{
		OLED_Setpos(page+i,colum);
		for(j = 0 ; j < 8 ; j++)
		{
			OLED_WR_Dat(buff[j+8*i]);
		}
	}
}

/*
	显示中文
	page---页地址（0-7）
	colum--列地址（0-128）
	备注:显示没有取模的汉字，汉字是显示不出来的，添加新汉字的方法：
	1、取模16x16汉字(汉字字模  取模方式：阴码、列行、逆向)
	2、将取模好的好的数据粘贴到dzk.h文件的DZK[]数组最后
	3、在Hz_Index[]数组后面加上取模数据的对应汉字
	4、更改dzk.h文件中GB16_NUM宏定义的数值，这个宏的值就是Hz_Index[]中总的汉字个数
*/
void OledDisplayChinese(char page,char colum,unsigned char *font)
{
	u8 buff[32] = {0};
	u8 x,i,j;

	for (x=0;x<GB16_NUM;x++) {
		if ((*font == Hz_Index[x*2]) && (*(font+1) == Hz_Index[x*2+1]))
		{
			/* 读取字模数据 */
			memcpy((char *)buff,&DZK[32*x],32);
			/* LCD液晶屏显示 */
			for(i = 0; i <2; i++)                
			{
				OLED_Setpos(page+i,colum);
				for(j = 0; j < 16; j++)
				{
					OLED_WR_Dat(buff[j+16*i]);
				}
			}
		}
	}
}


/* 	显示中英文字符串 
	page---页地址（0-7）
	colum--列地址（0-128）
*/
void OledDisplayString(char page,char colum,unsigned char *str)
{
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] <= 0xa0){	//显示英文
			OledDisplayAscii(page,colum,str[i]);
			colum += 8;
			i++;
			// add
			
			if(colum>=127){
				colum=0;
				page+=2;
			}
			//end
		} else {				//显示中文
			OledDisplayChinese(page,colum, &str[i]);
			colum += 16;
			i += 2;
			//add
			if(colum>=127){
				colum=0;
				page+=2;
			}
			// end
		}
	}	
}

/*********OLED设置像素点函数*************
参数说明：
x:x轴坐标 0-127
y:y轴坐标 0-7
*****************************************/
void oled_setpos(u8 x,u8 y)
{
	x += 2;
	//设置x轴
	OLED_WR_Com((x & 0xf0)>>4|0x10);   //高四位
	OLED_WR_Com((x & 0xf0)>>4|0x10);   //高四位((x & 0x0F)|0x00);      //低四位
	//设置y轴
	OLED_WR_Com(0xB0 + y);
}



/**********************************************************************************************************
* 函数名    ：   OLED_drawBMP
* 功能描述  ：   功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7
* 参数说明  ：
* 返回值说明：   none
**********************************************************************************************************/ 
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,u8 BMP[])
{
	unsigned int j = 0;
	unsigned char x,y;

	for(y = y0; y < y1;y++)
	{
		oled_setpos(x0,y);
		for(x = x0; x < x1; x++)
		{
			OLED_WR_Dat(BMP[j++]);	    	
		}
	}
} 


