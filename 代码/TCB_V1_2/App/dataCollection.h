/***
	************************************************************************************************************************************************************************************************
	*	@file  	dataCollection.h
	*	@version V1.0
	*  	@date    2021-6-15
	*	@author  YHF
	*	@brief   data collection	
   **********************************************************************************************************************************************************************************************
   *  @description
   *	里面包含了电压和电流信号的采集、温度的采集
   *	这里用到了 ADC1
	*
***/

#ifndef __DATA_COLLECTION_H
#define __DATA_COLLECTION_H

#include "cmsis_os.h"
//设备状态
typedef enum
{
    DEVICE_OK = 0,		//设备正常
    REFRIG_ABN1 = 1,	//前置制冷异常
    REFRIG_ABN2 = 2,	//后置制冷异常
    UNDERV_P = 3,		//欠压保护
	OVERVO_P = 4,		//过压保护
	OVERCU_P = 5,		//过流保护
	TEMPER_ABN1 = 6,	//内部温度传感器异常
	TEMPER_ABN2 = 7,	//外部温度传感器异常
	LOCK_ABN = 8,		//电磁锁异常
	FAN_ABN = 9			//对流风扇异常
}DEVICE_STATUS_TypeDef;



extern double Temper;



#endif //__DATA_COLLECTION_H

