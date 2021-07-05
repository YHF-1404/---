/***
	************************************************************************************************************************************************************************************************
	*	@file  	temperControl.h
	*	@version V1.0
	*  	@date    2021-6-15
	*	@author  YHF
	*	@brief   控制舵机	
   **********************************************************************************************************************************************************************************************
   *  @description
   *	用于控制舵机
	*
***/

#ifndef __TEMPER_CONTROL_H
#define __TEMPER_CONTROL_H

#include "main.h"

typedef struct 
{
	double Kp;
	double Ti;
	double Td;
	double EK_0;
	double EK_1;
	double Inc;
	double offset;
	double output;
	uint16_t outMax;
	uint16_t outMin;
}temperPID_TypeDef;



void  temper_PwmSetPulse1 (uint16_t pulse);
void  temper_PwmSetPulse2 (uint16_t pulse);

void pid_Init(temperPID_TypeDef *pid);
uint16_t pid_calc(temperPID_TypeDef *pid, double ref, double fdb);

#endif //__TEMPER_CONTROL_H

