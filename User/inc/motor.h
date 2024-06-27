#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stm32f4xx.h"

#define DUTY_RATIO 80  //ռ�ձ�

#define MOTOR_PSC    83
#define MOTOR_ARR    100

#define LSTBY_STATUS(x) (x ? GPIO_SetBits(GPIOD,GPIO_Pin_14) : GPIO_ResetBits(GPIOD,GPIO_Pin_14))
#define RSTBY_STATUS(x) (x ? GPIO_SetBits(GPIOE,GPIO_Pin_13) : GPIO_ResetBits(GPIOE,GPIO_Pin_13))
#define LR_FORWARD()  do{GPIO_SetBits(GPIOD,GPIO_Pin_8);   \
											GPIO_ResetBits(GPIOD,GPIO_Pin_9); }while(0)   //��ǰǰ��1   0

#define LR_BACKWARD() do{GPIO_ResetBits(GPIOD,GPIO_Pin_8); \
											GPIO_SetBits(GPIOD,GPIO_Pin_9); }while(0)     //��ǰ����0   1

#define LF_FORWARD()  do{GPIO_SetBits(GPIOD,GPIO_Pin_11);   \
											GPIO_ResetBits(GPIOD,GPIO_Pin_10); }while(0)  //���ǰ��1   0

#define LF_BACKWARD() do{GPIO_ResetBits(GPIOD,GPIO_Pin_11); \
											GPIO_SetBits(GPIOD,GPIO_Pin_10); }while(0)    //������0    1

#define RF_FORWARD()  do{GPIO_SetBits(GPIOE,GPIO_Pin_15);   \
											GPIO_ResetBits(GPIOE,GPIO_Pin_14); }while(0)  //��ǰǰ��

#define RF_BACKWARD() do{GPIO_ResetBits(GPIOE,GPIO_Pin_15); \
											GPIO_SetBits(GPIOE,GPIO_Pin_14); }while(0)    //��ǰ����

#define RR_FORWARD() do{GPIO_ResetBits(GPIOE,GPIO_Pin_12); \
											GPIO_SetBits(GPIOE,GPIO_Pin_11); }while(0)    //�Һ�ǰ��

#define RR_BACKWARD()  do{GPIO_SetBits(GPIOE,GPIO_Pin_12);   \
											GPIO_ResetBits(GPIOE,GPIO_Pin_11); }while(0)  //�Һ����

#define LF_SHORTBRAKE() do{GPIO_SetBits(GPIOD,GPIO_Pin_9);   \
											GPIO_SetBits(GPIOD,GPIO_Pin_8); }while(0)     //��ǰɲ��
#define LR_SHORTBRAKE()  do{GPIO_SetBits(GPIOD,GPIO_Pin_10);   \
											GPIO_SetBits(GPIOD,GPIO_Pin_11); }while(0)    //���ɲ��
#define RF_SHORTBRAKE() do{GPIO_SetBits(GPIOE,GPIO_Pin_14); \
											GPIO_SetBits(GPIOE,GPIO_Pin_15); }while(0)    //��ǰɲ��
#define RR_SHORTBRAKE()  do{GPIO_SetBits(GPIOE,GPIO_Pin_12);   \
											GPIO_SetBits(GPIOE,GPIO_Pin_11); }while(0)    //�Һ�ɲ��

void Motor_Init(void);
void Motor_GPIOInit(void);
void Motor_PwmInit(void);
void Motor_Forward(void);
void Motor_Backward(void);
void Motor_MoveRight(void);
void Motor_MoveLeft(void);	
void Motor_TurnRight(void);
void Motor_TurnLeft(void);		
void Motor_SpinRight(void);
void Motor_SpinLeft(void);								
void Motor_Stop(void);
void Motor_StepBack(void);
void Motor_SetSpeed(uint8_t speed);


#endif
