/***
	************************************************************************************************************************************************************************************************
	*	@file  	lockControl.c
	*	@version V1.0
	*  	@date    2021-6-15
	*	@author  YHF
	*	@brief   	
   **********************************************************************************************************************************************************************************************
   *  @description
   * 	应用舵机来开关锁
   *	用到了TIM1通道1控制舵机
   *	频率50Hz
	*
***/

#include "lockControl.h"
#include "tim.h"
#include "freertosTask.h"
#include "uartApp.h"

//根据舵机不同开角对应占空比值确定占空比
#define openLockPulse 5			//开锁时占空
#define offLockPulse 10			//关锁时占空
#define lock_PwmPeriod 20000	//pwm分辨率

extern TIM_HandleTypeDef htim1;
extern setVal_Typedef setVal;
extern void StartLockTask(void *argument);

//锁初始化函数
void lock_Init(void)
{
	uint8_t pwm_value = 10;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	lock_PwmSetPulse(pwm_value);
	
}
//设置占空比
void  lock_PwmSetPulse (uint8_t pulse)
{
	uint16_t compareValue ;

	compareValue = pulse * lock_PwmPeriod / 100; //

	TIM1->CCR1 = compareValue; 			// 
}
//开锁
void openLock(void)
{
    lock_PwmSetPulse(openLockPulse);
}
//关锁
void offLock(void)
{
    lock_PwmSetPulse(offLockPulse);
}

/**
* @brief Function implementing the lockTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLockTask */
void StartLockTask(void *argument)
{
  /* USER CODE BEGIN StartLockTask */
	  lock_Init();
  /* Infinite loop */
  for(;;)
  {
	  if(setVal.onOffLock)	//接收到开锁命令
	  {
		  openLock();
	  }
	  if(setVal.onOffLock == 0)	//接收到关锁命令
	  {
		  offLock();
	  }
      osDelay(100);
  }
  /* USER CODE END StartLockTask */
}
