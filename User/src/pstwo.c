#include "pstwo.h"
#include "voice.h"
typedef uint8_t u8;
typedef uint16_t u16;

static uint16_t Handkey;
static uint8_t PS2_Comd[2]={0x01,0x42};	//开始命令。请求数据
static uint8_t PS2_Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //数据存储数组
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
};	//按键值与按键明

	
/* 手柄接口初始化 */
void PS2_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* 使用GPIOB时钟 */
	RCC_AHB1PeriphClockCmd(PS2_CS_CLK|PS2_SCLK_CLK|PS2_MOSI_CLK|PS2_MISO_CLK, ENABLE);
	
	/* 配置PS2_MISO引脚：下拉输入模式 */
	GPIO_InitStruct.GPIO_Pin = PS2_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(PS2_MISO_PORT, &GPIO_InitStruct);

	/* 配置PS2_MOSI引脚：普通推挽模式 */
	GPIO_InitStruct.GPIO_Pin = PS2_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PS2_MOSI_PORT, &GPIO_InitStruct);	
	
	/* 配置PS2_SCLK引脚：普通推挽模式 */
	GPIO_InitStruct.GPIO_Pin = PS2_SCLK_PIN;
	GPIO_Init(PS2_SCLK_PORT, &GPIO_InitStruct);	
	
	/* 配置PS2_CS引脚：普通推挽模式 */
	GPIO_InitStruct.GPIO_Pin = PS2_CS_PIN;
	GPIO_Init(PS2_CS_PORT, &GPIO_InitStruct);	
}

/* 向手柄发送命令 */
static void PS2_Cmd(u8 CMD)
{
	volatile u16 ref=0x01;
	PS2_Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1)
	{
		if(ref&CMD)
		{
			DO_H;                   //输出一位控制位
		}
		else DO_L;

		CLK_H;                        //时钟拉高
		delay_us(10);
		CLK_L;
		delay_us(10);
		CLK_H;
		if(DI)
			PS2_Data[1] = ref|PS2_Data[1];
	}
	delay_us(16);
}



//判断是否为红灯模式
//返回值；0，红灯模式
//		  其他，其他模式
uint8_t PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(PS2_Comd[0]);  //开始命令
	PS2_Cmd(PS2_Comd[1]);  //请求数据
	CS_H;
	if( PS2_Data[1] == 0X73)   return 0 ;
	else return 1;
}
//读取手柄数据
static void PS2_ReadData(void)
{
	volatile u8 byte=0;
	volatile u16 ref=0x01;

	CS_L;

	PS2_Cmd(PS2_Comd[0]);  //开始命令
	PS2_Cmd(PS2_Comd[1]);  //请求数据

	for(byte=2;byte<9;byte++)          //开始接受数据
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

//清除数据缓冲区
static void PS2_ClearData()
{
	u8 a;
	for(a=0;a<9;a++)
		PS2_Data[a]=0x00;
}

//对读出来的PS2的数据进行处理      只处理了按键部分         默认数据是红灯模式  只有一个按键按下时
//按下为0， 未按下为1s
uint8_t PS2_DataKey(void)
{
	u8 index;
	static uint8_t first_press = 0;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(PS2_Data[4]<<8)|PS2_Data[3];     //这是16个按键  按下为0， 未按下为1
	if (Handkey==0) return 0;                 //按键全部按下才为0（）
	
	for(index=0;index<16;index++)
	{	    
		if((Handkey&(1<<(PS2_MASK[index]-1)))==0) 
		{
			if (index+1==PSB_SELECT && first_press==0) 
			{	
				//当键值为SELECT且首次按下时	
				first_press = 1;
			}	
			else if (index+1==PSB_SELECT && first_press==1 ) 
			{	
				//当键值为SELECT且一直按下时
				return 0;
			}
			return index+1;
		}
	}
	first_press = 0;
	return 0;          //没有任何按键按下
}

//得到一个摇杆的模拟量	 范围0~256
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
            Motor_Forward();    // 前进
            Voice_Play(1);      // 播放前进音频
            break;
        case PSB_PAD_DOWN:
            Motor_Backward();   // 后退
            Voice_Play(2);      // 播放后退音频
            break;
        case PSB_PAD_LEFT:
            Motor_MoveLeft();   // 左移
            Voice_Play(3);      // 播放左移音频
            break;
        case PSB_PAD_RIGHT:
            Motor_MoveRight();  // 右移
            Voice_Play(4);      // 播放右移音频
            break;
        case PSB_R1:
            Motor_TurnRight();  // 右转
            Voice_Play(8);      // 播放右转音频
            break;
        case PSB_L1:
            Motor_TurnLeft();   // 左转
            Voice_Play(7);      // 播放左转音频
            break;
        case PSB_R2:
            Motor_SpinRight();  // 右旋
            Voice_Play(6);      // 播放右旋音频
            break;
        case PSB_L2:
            Motor_SpinLeft();   // 左旋
            Voice_Play(5);      // 播放左旋音频
            break;
        default:
            Motor_Stop();       // 默认停止
            Voice_Play(9);      // 播放停止音频
            break;
    }
}

// void PstwoControl(void)
// {
//     if (sys_now() % 10 != 0)
//         return;

//     uint8_t key = PS2_DataKey();

//     switch (key)
//     {
//         case PSB_PAD_UP:
//             Motor_Forward();    // 前进
//             break;
//         case PSB_PAD_DOWN:
//             Motor_Backward();   // 后退
//             break;
//         case PSB_PAD_LEFT:
//             Motor_MoveLeft();   // 左移
//             break;
//         case PSB_PAD_RIGHT:
//             Motor_MoveRight();  // 右移
//             break;
//         case PSB_R1:
//             Motor_TurnRight();  // 右转
//             break;
//         case PSB_L1:
//             Motor_TurnLeft();   // 左转
//             break;
//         case PSB_R2:
//             Motor_SpinRight();  // 右旋
//             break;
//         case PSB_L2:
//             Motor_SpinLeft();   // 左旋
//             break;
//         default:
//             Motor_Stop();       // 默认停止
//             break;
//     }
// }

static uint32_t sys_ticks;
/* 获取系统时间毫秒数 (lwip协议栈要求实现的函数) */
// 该函数必须保证: 除非定时器溢出, 否则后获取的时间必须大于先获取的时间
uint32_t sys_now(void)
{
  return sys_ticks;
}

/* 初始化系统时间毫秒计数器 */
void sys_now_init(void)
{
  SysTick_Config(SystemCoreClock / 1000);
}		



