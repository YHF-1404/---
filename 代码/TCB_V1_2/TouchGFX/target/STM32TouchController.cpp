/**
  ******************************************************************************
  * File Name          : STM32TouchController.cpp
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* USER CODE BEGIN STM32TouchController */

#include <STM32TouchController.hpp>
#include "touch_480x272.h"
void STM32TouchController::init()
{
    /**
     * Initialize touch controller and driver
     *
     */
    
	  Touch_Init();
	
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    /**
     * By default sampleTouch returns false,
     * return true if a touch has been detected, otherwise false.
     *
     * Coordinates are passed to the caller by reference by x and y.
     *
     * This function is called by the TouchGFX framework.
     * By default sampleTouch is called every tick, this can be adjusted by HAL::setTouchSampleRate(int8_t);
     *
     */

	Touch_Scan();	//
	if(touchInfo.flag == 1)
	{
//	   LCD_DisplayNumber( 80,114,touchInfo.x[0],4);	// 鏄剧ず锟???1缁勫潗锟???
//		LCD_DisplayNumber(200,114,touchInfo.y[0],4);
//
//		LCD_DisplayNumber( 80,144,touchInfo.x[1],4);	// 鏄剧ず锟???2缁勫潗锟???
//		LCD_DisplayNumber(200,144,touchInfo.y[1],4);
//
//		LCD_DisplayNumber( 80,174,touchInfo.x[2],4);	// 鏄剧ず锟???3缁勫潗锟???
//		LCD_DisplayNumber(200,174,touchInfo.y[2],4);
//
//		LCD_DisplayNumber( 80,204,touchInfo.x[3],4);	// 鏄剧ず锟???4缁勫潗锟???
//		LCD_DisplayNumber(200,204,touchInfo.y[3],4);
//
//		LCD_DisplayNumber( 80,234,touchInfo.x[4],4);	// 鏄剧ず锟???5缁勫潗锟???
//		LCD_DisplayNumber(200,234,touchInfo.y[4],4);
		x = touchInfo.x[0];
		y = touchInfo.y[0];
	    return true;
	}
	else
	{
	    return false;
	}
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
