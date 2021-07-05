/***
	************************************************************************************************************************************************************************************************
	*	@file  	temperControl.c
	*	@version V1.0
	*  	@date    2021-6-15
	*	@author  YHF
	*	@brief   温度控制
   **********************************************************************************************************************************************************************************************
   *  @description
   *	用到了TIM12
   *    TIM12_CH1控制制冷片1的电流
   *    TIM12_CH2控制制冷片2的电流
   * 	使用PWM模拟DAC的方式，PWM输出引脚接入阻容滤波网络生产对应电压值 再通过电流反馈环路对电流进行控制
   * 	使用PID环路对温度进行控制
	*
***/
#include "temperControl.h"
#include "tim.h"
#include "freertosTask.h"
#include "uartApp.h"
#include "dataCollection.h"
//定义输出范围
#define out_max 3000
#define out_min 0

//PID参数
#define temper_P 3000
#define temper_I 4
#define temper_D 0

temperPID_TypeDef temper_pid1;
extern TIM_HandleTypeDef htim12;
extern setVal_Typedef setVal;
extern void StartTemperTask(void *argument);

void temper_Init(void)
{
	uint8_t pwm_value = 0;
	HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
	temper_PwmSetPulse1(pwm_value);
	temper_PwmSetPulse1(pwm_value);
}

void  temper_PwmSetPulse1 (uint16_t pulse)
{
	TIM12->CCR1 = pulse; 			// 
}

void  temper_PwmSetPulse2 (uint16_t pulse)
{
	TIM12->CCR2 = pulse; 			// 
}


void StartTemperTask(void *argument)
{
	uint16_t outControl = 0;
    temper_Init();
	pid_Init(&temper_pid1);
    osDelay(3000);
    // temper_PwmSetPulse1(1000);
    // temper_PwmSetPulse2(3000);
	setVal.interTemper = 50;
    for(;;)
    {
		outControl = pid_calc(&temper_pid1, setVal.interTemper, Temper);
		temper_PwmSetPulse1(outControl);
		temper_PwmSetPulse2(outControl);
        osDelay(1500);	//1.5s计算一次
    }
}

//PID初始化
void pid_Init(temperPID_TypeDef *pid)
{
	if(pid)
	{
		pid->Kp = temper_P;
		pid->Ti = temper_I;
		pid->Td = temper_D;
		pid->EK_0 = 0;
		pid->EK_1 = 0;
		pid->Inc = 0;
		pid->offset = 0;
		pid->output = 0;
	}
}
//PID计算
uint16_t pid_calc(temperPID_TypeDef *pid, double ref, double fdb)
{
	pid->EK_0 = fdb - ref;		//计算误差
	pid->Inc = pid->EK_0 + pid->Inc;	
	if(pid->Inc > 300) pid->Inc = 300;
	if(pid->Inc < -300) pid->Inc = -300;
	pid->output = pid->Kp * pid->EK_0 +(pid->Ti * pid->Inc) + pid->Td * (pid->EK_1 -pid->EK_0);//计算结果
	pid->output = pid->output + pid->offset;
	if(pid->output > out_max)pid->output = out_max;	//最大限制幅度
	if(pid->output < out_min)pid->output = out_min;	//最小限制幅度
	pid->EK_1 = pid->EK_0;	//保存误差
	return (uint16_t)pid->output;
}

