#include "pstwo.h"
typedef uint8_t u8;
typedef uint16_t u16;

static uint16_t Handkey;
static uint8_t PS2_Comd[2]={0x01,0x42};	//��ʼ�����������
static uint8_t PS2_Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //���ݴ洢����
static uint16_t PS2_MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
};	//����ֵ�밴����

	
/* �ֱ��ӿڳ�ʼ�� */
void PS2_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* ʹ��GPIOBʱ�� */
	RCC_AHB1PeriphClockCmd(PS2_CS_CLK|PS2_SCLK_CLK|PS2_MOSI_CLK|PS2_MISO_CLK, ENABLE);
	
	/* ����PS2_MISO���ţ���������ģʽ */
	GPIO_InitStruct.GPIO_Pin = PS2_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(PS2_MISO_PORT, &GPIO_InitStruct);

	/* ����PS2_MOSI���ţ���ͨ����ģʽ */
	GPIO_InitStruct.GPIO_Pin = PS2_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PS2_MOSI_PORT, &GPIO_InitStruct);	
	
	/* ����PS2_SCLK���ţ���ͨ����ģʽ */
	GPIO_InitStruct.GPIO_Pin = PS2_SCLK_PIN;
	GPIO_Init(PS2_SCLK_PORT, &GPIO_InitStruct);	
	
	/* ����PS2_CS���ţ���ͨ����ģʽ */
	GPIO_InitStruct.GPIO_Pin = PS2_CS_PIN;
	GPIO_Init(PS2_CS_PORT, &GPIO_InitStruct);	
}

/* ���ֱ��������� */
static void PS2_Cmd(u8 CMD)
{
	volatile u16 ref=0x01;
	PS2_Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1)
	{
		if(ref&CMD)
		{
			DO_H;                   //���һλ����λ
		}
		else DO_L;

		CLK_H;                        //ʱ������
		delay_us(10);
		CLK_L;
		delay_us(10);
		CLK_H;
		if(DI)
			PS2_Data[1] = ref|PS2_Data[1];
	}
	delay_us(16);
}



//�ж��Ƿ�Ϊ���ģʽ
//����ֵ��0�����ģʽ
//		  ����������ģʽ
uint8_t PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(PS2_Comd[0]);  //��ʼ����
	PS2_Cmd(PS2_Comd[1]);  //��������
	CS_H;
	if( PS2_Data[1] == 0X73)   return 0 ;
	else return 1;
}
//��ȡ�ֱ�����
static void PS2_ReadData(void)
{
	volatile u8 byte=0;
	volatile u16 ref=0x01;

	CS_L;

	PS2_Cmd(PS2_Comd[0]);  //��ʼ����
	PS2_Cmd(PS2_Comd[1]);  //��������

	for(byte=2;byte<9;byte++)          //��ʼ��������
	{
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			CLK_H;
			delay_us(10);
			CLK_L;
			delay_us(10);
			CLK_H;
		    if(DI)
		      PS2_Data[byte] = ref|PS2_Data[byte];
		}
		delay_us(16);
	}
	CS_H;	
}

//������ݻ�����
static void PS2_ClearData()
{
	u8 a;
	for(a=0;a<9;a++)
		PS2_Data[a]=0x00;
}

//�Զ�������PS2�����ݽ��д���      ֻ�����˰�������         Ĭ�������Ǻ��ģʽ  ֻ��һ����������ʱ
//����Ϊ0�� δ����Ϊ1s
uint8_t PS2_DataKey(void)
{
	u8 index;
	static uint8_t first_press = 0;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(PS2_Data[4]<<8)|PS2_Data[3];     //����16������  ����Ϊ0�� δ����Ϊ1
	if (Handkey==0) return 0;                 //����ȫ�����²�Ϊ0����
	
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(PS2_MASK[index]-1)))==0) 
		{
			if (index+1==PSB_SELECT && first_press==0) 
			{	
				//����ֵΪSELECT���״ΰ���ʱ	
				first_press = 1;
			}	
			else if (index+1==PSB_SELECT && first_press==1 ) 
			{	
				//����ֵΪSELECT��һֱ����ʱ
				return 0;
			}
			return index+1;
		}
	}
	first_press = 0;
	return 0;          //û���κΰ�������
}

//�õ�һ��ҡ�˵�ģ����	 ��Χ0~256
uint8_t PS2_AnologData(uint8_t button)
{
	return PS2_Data[button];
}


u8 err[50] = {0};

//void PstwoControl(void)
//{
//	if (sys_now()%10!=0) return;
//	uint8_t key = PS2_DataKey();
//	
//	switch (key) 
//	{
//		case PSB_PAD_UP: 	Motor_Forward(); 	break;
//		
//		default: 	        Motor_Stop(); 	break;
//	}
//}
void PstwoControl(void)
{
    if (sys_now() % 10 != 0)
        return;

    uint8_t key = PS2_DataKey();

    switch (key)
    {
        case PSB_PAD_UP:
            Motor_Forward();    // ǰ��
            break;
        case PSB_PAD_DOWN:
            Motor_Backward();   // ����
            break;
        case PSB_PAD_LEFT:
            Motor_MoveLeft();   // ����
            break;
        case PSB_PAD_RIGHT:
            Motor_MoveRight();  // ����
            break;
        case PSB_R1:
            Motor_TurnRight();  // ��ת
            break;
        case PSB_L1:
            Motor_TurnLeft();   // ��ת
            break;
        case PSB_R2:
            Motor_SpinRight();  // ����
            break;
        case PSB_L2:
            Motor_SpinLeft();   // ����
            break;
        default:
            Motor_Stop();       // Ĭ��ֹͣ
            break;
    }
}

static uint32_t sys_ticks;
/* ��ȡϵͳʱ������� (lwipЭ��ջҪ��ʵ�ֵĺ���) */
// �ú������뱣֤: ���Ƕ�ʱ�����, ������ȡ��ʱ���������Ȼ�ȡ��ʱ��
uint32_t sys_now(void)
{
  return sys_ticks;
}

/* ��ʼ��ϵͳʱ���������� */
void sys_now_init(void)
{
  SysTick_Config(SystemCoreClock / 1000);
}		



