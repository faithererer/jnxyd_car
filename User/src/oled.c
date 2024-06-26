#include "oled.h"
#include "DZK.h"
#include "font.h"
#include "timer.h"
#include <string.h>

static void OLED_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	/*!< ʹ�� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd (OLED_CS_CLK|OLED_DC_CLK|OLED_RST_CLK|OLED_SCLK_CLK|OLED_MOSI_CLK, ENABLE);

	/*!< ���� OLED_CS ����: ��ͨ������� */
	GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(OLED_CS_PORT, &GPIO_InitStructure);
	
	/*!< ���� OLED_DC ����: ��ͨ������� */
	GPIO_InitStructure.GPIO_Pin = OLED_DC_PIN;
	GPIO_Init(OLED_DC_PORT, &GPIO_InitStructure);
	
	/*!< ���� OLED_RST ����: ��ͨ������� */
	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
	GPIO_Init(OLED_RST_PORT, &GPIO_InitStructure);
	
	/*!< ���� OLED_SCLK ����: ����ΪSPI1 */
	GPIO_InitStructure.GPIO_Pin = OLED_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(OLED_SCLK_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(OLED_SCLK_PORT, OLED_SCLK_PINSOURCE, OLED_SCLK_AF);

	/*!< ���� OLED_MOSI ����: ����ΪSPI1 */
	GPIO_InitStructure.GPIO_Pin = OLED_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(OLED_MOSI_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(OLED_MOSI_PORT, OLED_MOSI_PINSOURCE, OLED_MOSI_AF);
}

static void OLED_SPIInit(void)
{
	SPI_InitTypeDef SPI_InitStruct = {0};
	/*!< ʹ��SPIʱ�� */
	OLED_SPI_CLK_INIT(OLED_SPI_CLK,ENABLE);
	/*!< SPI��ʼ��  */
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
	/* ʹ��SPI */
	SPI_Cmd(OLED_SPI, ENABLE);
}

/************************************************
���ܣ�SPI�����ֽں���
************************************************/
static char SPI_WiteByte(uint8_t data)
{
	uint8_t retry = 0;
	while (SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET) 
	{	//�ȴ���������
		retry++;
		if (retry > 200)	
			return -1;
	}
	SPI_I2S_SendData(OLED_SPI, data); //ͨ������ SPI1 ����һ������
	return 0;
//	while( ! SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));  //�ȴ����ݷ������
//	SPI_I2S_SendData(SPI1,data);
//	while(! SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));  //�ȴ��������
//	return SPI_I2S_ReceiveData(SPI1);
}

#define OLED_WriteByte(data)	SPI_WiteByte(data)

/* OLEDд���� */
static void OLED_WR_Com(uint8_t Com)
{
	OLED_CS_Low();
	OLED_DC_Low();
	OLED_WriteByte(Com);
	OLED_CS_High();
}

/* OLEDд���� */
static void OLED_WR_Dat(uint8_t Dat)
{
	OLED_CS_Low();
	OLED_DC_High();
	OLED_WriteByte(Dat);
	OLED_CS_High();
}

/*
	���ã����
	������page--�ڼ�ҳ   colum--�ڼ�ҳ
	����ֵ����
*/
static void OLED_Setpos(int page,int colum)
{
   OLED_WR_Com(0xB0+page);
   OLED_WR_Com(((colum&0xf0)>>4)|0x10);//&�������ȼ��Ƚϵ� ��Ҫ������
   OLED_WR_Com((colum&0x0f));
}

/* OLED�������� */
void OLED_Clear(void)
{
	int x , y;

	for(y = 0;y <= 7;y++)
	{
		OLED_Setpos(y,0);
		for(x = 0;x <= 128;x++)
		{
			OLED_WR_Dat(0x00);    //д������0������	    	
		}
	}	
}

/* OLED��ʼ�� */
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
	OLED_WR_Com(0xA1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Com(0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
	��ʾAscii���ַ�
	page---ҳ��ַ��0-7��
	colum--�е�ַ��0-128��
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
	��ʾ����
	page---ҳ��ַ��0-7��
	colum--�е�ַ��0-128��
	��ע:��ʾû��ȡģ�ĺ��֣���������ʾ�������ģ�����º��ֵķ�����
	1��ȡģ16x16����(������ģ  ȡģ��ʽ�����롢���С�����)
	2����ȡģ�õĺõ�����ճ����dzk.h�ļ���DZK[]�������
	3����Hz_Index[]����������ȡģ���ݵĶ�Ӧ����
	4������dzk.h�ļ���GB16_NUM�궨�����ֵ��������ֵ����Hz_Index[]���ܵĺ��ָ���
*/
void OledDisplayChinese(char page,char colum,unsigned char *font)
{
	u8 buff[32] = {0};
	u8 x,i,j;

	for (x=0;x<GB16_NUM;x++) {
		if ((*font == Hz_Index[x*2]) && (*(font+1) == Hz_Index[x*2+1]))
		{
			/* ��ȡ��ģ���� */
			memcpy((char *)buff,&DZK[32*x],32);
			/* LCDҺ������ʾ */
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


/* 	��ʾ��Ӣ���ַ��� 
	page---ҳ��ַ��0-7��
	colum--�е�ַ��0-128��
*/
void OledDisplayString(char page,char colum,unsigned char *str)
{
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] <= 0xa0){	//��ʾӢ��
			OledDisplayAscii(page,colum,str[i]);
			colum += 8;
			i++;
			// add
			
			if(colum>=127){
				colum=0;
				page+=2;
			}
			//end
		} else {				//��ʾ����
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

/*********OLED�������ص㺯��*************
����˵����
x:x������ 0-127
y:y������ 0-7
*****************************************/
void oled_setpos(u8 x,u8 y)
{
	x += 2;
	//����x��
	OLED_WR_Com((x & 0xf0)>>4|0x10);   //����λ
	OLED_WR_Com((x & 0xf0)>>4|0x10);   //����λ((x & 0x0F)|0x00);      //����λ
	//����y��
	OLED_WR_Com(0xB0 + y);
}



/**********************************************************************************************************
* ������    ��   OLED_drawBMP
* ��������  ��   ������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
* ����˵��  ��
* ����ֵ˵����   none
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


